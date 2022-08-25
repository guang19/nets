//
// Created by guang19
//

#include "nets/net/core/SocketChannel.h"

#include <climits>
#include <thread>

#include "nets/base/log/Logging.h"
#include "nets/net/core/EventLoop.h"
#include "nets/net/exception/ChannelRegisterException.h"

namespace nets::net
{
    namespace
    {
        const SocketChannel::SizeType RecvBufferSize = DefaultTcpSockRecvBufferSize >> 1;
        constexpr ::int32_t MaxCountOfWriteVOnce = IOV_MAX >> 2;
    } // namespace

    SocketChannel::SocketChannel(FdType sockFd, const InetSockAddress& localAddress, const InetSockAddress& peerAddress,
                                 EventLoopRawPtr eventLoop)
        : Channel(eventLoop), sockFd_(sockFd), localAddress_(localAddress), peerAddress_(peerAddress), writeBuffer_(),
          state_(ChannelState::INACTIVE), channelHandlerPipeline_(this)
    {
    }

    SocketChannel::~SocketChannel()
    {
        assert(state_ == ChannelState::INACTIVE);
        socket::closeFd(sockFd_);
    }

    FdType SocketChannel::fd() const
    {
        return sockFd_;
    }

    void SocketChannel::handleReadEvent()
    {
        if (state_ == ChannelState::INACTIVE)
        {
            LOGS_WARN << "SocketChannel handleReadEvent,but wrong state " << state_;
            return;
        }
        ByteBuffer byteBuffer(RecvBufferSize);
        SSizeType bytes = doRead(byteBuffer);
        if (bytes > 0)
        {
            try
            {
                channelHandlerPipeline_.fireChannelRead(byteBuffer);
            }
            catch (const ::std::exception& exception)
            {
                channelHandlerPipeline_.fireExceptionCaught(exception);
            }
        }
        else if (bytes == 0)
        {
            // close_wait.the peer may shutdown write or close socket, the local can not close socket directly.
            // if the peer shutdown write, but the local can still send data, the peer can also read the data send by local.
            // so if the local still has data waiting to be sent, just shutdown read; if there is no data waiting to be sent,
            // then shutdown both
            if (writeBuffer_.empty())
            {
                shutdown();
            }
            else
            {
                shutdownRead();
                setEvents(EWriteEvent);
                modify();
            }
        }
        else
        {
            ::int32_t errNum = errno;
            handleReadError(errNum);
        }
    }

    void SocketChannel::handleWriteEvent()
    {
        if (state_ == ChannelState::INACTIVE)
        {
            LOGS_WARN << "SocketChannel handleWriteEvent,but wrong state " << state_;
            return;
        }
        assert(!writeBuffer_.empty());
        SizeType size = writeBuffer_.size();
        IoVecList ioves(size);
        SizeType totalBytes = 0;
        for (SizeType i = 0; i < size; ++i)
        {
            ioves[i].iov_base = writeBuffer_[i].data();
            ioves[i].iov_len = writeBuffer_[i].readableBytes();
            totalBytes += writeBuffer_[i].readableBytes();
        }
        assert(totalBytes > 0);
        SSizeType writtenBytes = writev(ioves, static_cast<::int32_t>(size));
        if (writtenBytes < 0)
        {
            ::int32_t errNum = errno;
            handleWriteError(errNum);
        }
        else
        {
            if (totalBytes == static_cast<SizeType>(writtenBytes))
            {
                writeBuffer_.clear();
                try
                {
                    channelHandlerPipeline_.fireChannelWriteComplete();
                }
                catch (const ::std::exception& exception)
                {
                    channelHandlerPipeline_.fireExceptionCaught(exception);
                }
                if (state_ == ChannelState::HALF_CLOSE)
                {
                    channelInActive();
                }
                else
                {
                    setEvents(EReadEvent);
                    modify();
                }
            }
            else
            {
                // if buffer has not been sent, clear the data that has been sent and send it next time
                removeSentBuffer(writtenBytes);
            }
        }
    }

    void SocketChannel::handleErrorEvent()
    {
        if (state_ == ChannelState::INACTIVE)
        {
            return;
        }
        LOGS_WARN << "SocketChannel handleErrorEvent,errNum=" << socket::getSockError(sockFd_);
        channelInActive();
    }

    void SocketChannel::setChannelOptions(const ChannelOptionList& channelOptions)
    {
        for (const auto& channelOption: channelOptions)
        {
            setChannelOption(channelOption);
        }
    }

    void SocketChannel::channelActive()
    {
        addEvent(EReadEvent);
        try
        {
            if (!registerTo())
            {
                THROW_FMT(ChannelRegisterException, "SocketChannel register failed");
            }
            state_ = ChannelState::ACTIVE;
            channelHandlerPipeline_.fireChannelConnect(localAddress_, peerAddress_);
        }
        catch (const ChannelRegisterException& exception)
        {
            if (isRegistered())
            {
                deregister();
            }
            LOGS_ERROR << "SocketChannel channelActive failed,cause " << exception.what();
        }
        // catch fireChannelConnect
        catch (const ::std::exception& exception)
        {
            channelHandlerPipeline_.fireExceptionCaught(exception);
        }
    }

    void SocketChannel::write(const void* message, SizeType length)
    {
        write(StringType(static_cast<const char*>(message), length));
    }

    void SocketChannel::write(const StringType& message)
    {
        if (eventLoop_->isInCurrentEventLoop())
        {
            doWrite(message.data(), message.length());
        }
        else
        {
            eventLoop_->execute(
                // msg must be value catch
                [this, msg = message]()
                {
                    doWrite(msg.data(), msg.length());
                });
        }
    }

    void SocketChannel::write(const ByteBuffer& message)
    {
        if (eventLoop_->isInCurrentEventLoop())
        {
            doWrite(message.data(), message.readableBytes());
        }
        else
        {
            eventLoop_->execute(
                // msg must be value catch
                [this, msg = message]()
                {
                    doWrite(msg.data(), msg.readableBytes());
                });
        }
    }

    void SocketChannel::shutdown()
    {
        shutdown(SHUT_RDWR);
        channelInActive();
    }

    void SocketChannel::shutdownRead()
    {
        shutdown(SHUT_RD);
        state_ = ChannelState::HALF_CLOSE;
    }

    void SocketChannel::shutdownWrite()
    {
        shutdown(SHUT_WR);
        state_ = ChannelState::HALF_CLOSE;
    }

    SSizeType SocketChannel::doRead(ByteBuffer& byteBuffer)
    {
        // read all at once
        SizeType expectedReadLen = byteBuffer.writableBytes();
        SSizeType readBytes = 0;
        while (true)
        {
            readBytes = byteBuffer.writeBytes(*this, expectedReadLen);
            if (readBytes > 0)
            {
                // adjust read bytes for next time
                SizeType adjustment = expectedReadLen - readBytes;
                if (adjustment > 0)
                {
                    expectedReadLen = adjustment;
                }
            }
            else if (readBytes < 0)
            {
                ::int32_t errNum = errno;
                if (errNum != EAGAIN && errNum != EINTR)
                {
                    errno = errNum;
                    return -1;
                }
                break;
            }
            else
            {
                // The peer may close or shut down the connection immediately after sending the data,
                // so the read data cannot be ignored
                if (byteBuffer.readableBytes() == 0)
                {
                    return 0;
                }
                break;
            }
        }
        return static_cast<SSizeType>(byteBuffer.readableBytes());
    }

    void SocketChannel::doWrite(const void* data, SizeType length)
    {
        if (state_ == ChannelState::INACTIVE)
        {
            LOGS_ERROR << "SocketChannel write,but wrong state " << state_;
            return;
        }
        if (length == 0)
        {
            return;
        }
        // the writeBuffer has residual data waiting to be sent, append the data to the end of the writeBuffer
        if (!writeBuffer_.empty())
        {
            appendBuffer(data, length);
        }
        else
        {
            // write directly
            doWriteDirectly(data, length);
        }
    }

    void SocketChannel::doWriteDirectly(const void* data, SizeType length)
    {
        SSizeType bytes = socket::write(sockFd_, data, length);
        if (bytes < 0)
        {
            ::int32_t errNum = errno;
            // ignore write EAGAIN
            if (errNum == EAGAIN || errNum == EINTR)
            {
                bytes = 0;
            }
            else
            {
                errno = errNum;
                handleWriteError(errNum);
            }
        }
        SizeType remainingBytes = length - bytes;
        // write complete
        if (remainingBytes == 0)
        {
            try
            {
                channelHandlerPipeline_.fireChannelWriteComplete();
            }
            catch (const ::std::exception& exception)
            {
                channelHandlerPipeline_.fireExceptionCaught(exception);
            }
        }
        // tcp send buffer size less than length,only part of it was sent
        else
        {
            appendBuffer(static_cast<const char*>(data) + bytes, remainingBytes);
        }
    }

    void SocketChannel::appendBuffer(const void* data, SizeType length)
    {
        // to avoid allocating new memory, check whether the writeBuffer back() can be directly appended
        if (writeBufferLastCanAppend(length))
        {
            writeBuffer_.back().writeBytes(data, length);
        }
        else
        {
            writeBuffer_.emplace_back(data, length);
        }
        if (!hasWriteEvent())
        {
            addEvent(EWriteEvent);
            modify();
        }
    }

    bool SocketChannel::writeBufferLastCanAppend(SizeType length)
    {
        if (!writeBuffer_.empty())
        {
            return writeBuffer_.back().writableBytes() >= length;
        }
        return false;
    }

    SSizeType SocketChannel::writev(const IoVecList& iovecs, ::int32_t count) const
    {
        SSizeType writtenBytes = 0;
        for (::int32_t writtenBlocks = 0; writtenBlocks < count;)
        {
            ::int32_t leftBlocks = count - writtenBlocks;
            SSizeType bytes = 0;
            SizeType expectedWriteLen = 0;
            // if leftBlocks less than CountOfWriteVOnce
            if (leftBlocks < MaxCountOfWriteVOnce)
            {
                for (::int32_t i = writtenBlocks; i < leftBlocks; ++i)
                {
                    expectedWriteLen += iovecs[i].iov_len;
                }
                bytes = socket::writev(sockFd_, &iovecs[writtenBlocks], leftBlocks);
                writtenBlocks += leftBlocks;
            }
            else
            {
                for (::int32_t i = writtenBlocks; i < MaxCountOfWriteVOnce; ++i)
                {
                    expectedWriteLen += iovecs[i].iov_len;
                }
                bytes = socket::writev(sockFd_, &iovecs[writtenBlocks], MaxCountOfWriteVOnce);
                writtenBlocks += MaxCountOfWriteVOnce;
            }
            if (bytes < 0)
            {
                ::int32_t errNum = errno;
                if (errNum == EAGAIN || errNum == EINTR)
                {
                    break;
                }
                return -1;
            }
            else
            {
                writtenBytes += bytes;
                // tcp send buffer may not be enough
                if (static_cast<SizeType>(bytes) < expectedWriteLen)
                {
                    break;
                }
            }
        }
        return writtenBytes;
    }

    void SocketChannel::removeSentBuffer(SSizeType writtenBytes)
    {
        for (auto it = writeBuffer_.begin(); it != writeBuffer_.end() && writtenBytes > 0; ++it)
        {
            if (it->readableBytes() <= static_cast<SizeType>(writtenBytes))
            {
                writeBuffer_.erase(it);
                writtenBytes -= it->readableBytes();
            }
            else
            {
                it->setReaderIndex(it->readerIndex() + writtenBytes);
                writtenBytes = 0;
            }
        }
    }

    void SocketChannel::handleReadError(::int32_t errNum)
    {
        switch (errNum)
        {
            // EAGAIN/EWOULDBLOCK is not an error
            // EWOULDBLOCK
            case EAGAIN:
            case EINTR:
                break;
            // error
            case EBADF:
            case EFAULT:
            case EINVAL:
            case EIO:
            case EPERM:
            case EISDIR:
            case ENOMEM:
            case ENOTCONN:
            case ENOTSOCK:
            default:
                LOGS_ERROR << "SocketChannel handleReadError unexpected error,errno=" << errNum;
                channelInActive();
                break;
        }
    }

    void SocketChannel::handleWriteError(::int32_t errNum)
    {
        switch (errNum)
        {
            // EAGAIN/EWOULDBLOCK is not an error
            // EWOULDBLOCK
            case EAGAIN:
            case EINTR:
                break;
            // error
            case EBADF:
            case EDESTADDRREQ:
            case EDQUOT:
            case EFAULT:
            case EFBIG:
            case EINVAL:
            case EIO:
            case ENOSPC:
            case EPERM:
            case EPIPE:
            default:
                LOGS_ERROR << "SocketChannel handleWriteError unexpected error,errno=" << errNum;
                channelInActive();
                break;
        }
    }

    void SocketChannel::channelInActive()
    {
        state_ = ChannelState::INACTIVE;
        try
        {
            channelHandlerPipeline_.fireChannelDisconnect();
        }
        catch (const ::std::exception& exception)
        {
            channelHandlerPipeline_.fireExceptionCaught(exception);
        }
        eventLoop_->addTask(
            [channel = shared_from_this()]()
            {
                channel->deregister();
                assert(!channel->isRegistered());
                assert(channel.use_count() == 1);
            });
    }

    void SocketChannel::shutdown(::int32_t how)
    {
        switch (how)
        {
            case SHUT_RD:
                socket::shutdown(sockFd_, how);
                break;
            case SHUT_WR:
            case SHUT_RDWR:
                if (!writeBuffer_.empty())
                {
                    writeBuffer_.clear();
                }
                socket::shutdown(sockFd_, how);
                break;
            default:
                LOGS_ERROR << "SocketChannel unknown shutdown operation";
                break;
        }
    }
} // namespace nets::net