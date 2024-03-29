// MIT License
//
// Copyright (c) 2022 guang19
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

// @brief The encapsulation of tcp socket operations

#include "nets/net/channel/SocketChannel.h"

#include <climits>
#include <thread>
#include <utility>

#include "nets/base/log/Logger.h"
#include "nets/net/core/EventLoop.h"
#include "nets/net/exception/ChannelRegisterException.h"

namespace nets
{
    namespace
    {
        constexpr SizeType kRecvBufferSize = 1024 << 4;
        constexpr Int32Type kMaxCountOfWriteVOnce = IOV_MAX >> 2;
    } // namespace

    SocketChannel::SocketChannel(FdType sockFd, const InetSockAddress& localAddress, const InetSockAddress& peerAddress,
                                 EventLoopRawPtr eventLoop)
        : Channel(eventLoop), sockFd_(sockFd), localAddress_(localAddress), peerAddress_(peerAddress), writeBuffer_(),
          state_(ChannelState::INACTIVE), channelHandlerPipeline_(new SocketChannelContext(this)), writeCompleteCallbacks_()
    {
    }

    SocketChannel::~SocketChannel()
    {
        // if disconnected normally,the status should be Inactive.
        // if the program is stopped due to external reasons such as interrupt signals,state may not be determined
        assert(state_ == ChannelState::INACTIVE);
        socket::closeFd(sockFd_);
    }

    FdType SocketChannel::fd() const
    {
        return sockFd_;
    }

    void SocketChannel::setChannelOptions(const ChannelOptionList& channelOptions)
    {
        for (const auto& channelOption : channelOptions)
        {
            setChannelOption(channelOption.first, channelOption.second);
        }
    }

    void SocketChannel::channelActive()
    {
        addEvent(kReadEvent);
        try
        {
            if (!registerTo())
            {
                THROW_FMT(ChannelRegisterException, "SocketChannel register failed");
            }
            state_ = ChannelState::ACTIVE;
            channelHandlerPipeline_.fireSocketChannelConnect(localAddress_, peerAddress_);
        }
        catch (const ChannelRegisterException& exception)
        {
            if (isRegistered())
            {
                deregister();
            }
            NETS_SYSTEM_LOG_ERROR << "SocketChannel channelActive failed,cause " << exception.what();
        }
    }

    void SocketChannel::write(const void* message, SizeType length, WriteCompleteCallback writeCompleteCallback)
    {
        write(StringType(static_cast<const char*>(message), length), std::move(writeCompleteCallback));
    }

    void SocketChannel::write(const StringType& message, WriteCompleteCallback writeCompleteCallback)
    {
        if (eventLoop_->isInCurrentEventLoop())
        {
            doWrite(message.data(), message.length(), ::std::move(writeCompleteCallback));
        }
        else
        {
            eventLoop_->execute(
                // msg must be value catch
                [&, writeCompleteCb = ::std::move(writeCompleteCallback), msg = message]()
                {
                    doWrite(msg.data(), msg.length(), writeCompleteCb);
                });
        }
    }

    void SocketChannel::write(const ByteBuffer& message, WriteCompleteCallback writeCompleteCallback)
    {
        if (eventLoop_->isInCurrentEventLoop())
        {
            doWrite(message.data(), message.readableBytes(), ::std::move(writeCompleteCallback));
        }
        else
        {
            eventLoop_->execute(
                // msg must be value catch
                [this, writeCompleteCb = ::std::move(writeCompleteCallback), msg = message]()
                {
                    doWrite(msg.data(), msg.readableBytes(), writeCompleteCb);
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

    void SocketChannel::handleReadEvent()
    {
        if (state_ == ChannelState::INACTIVE)
        {
            NETS_SYSTEM_LOG_WARN << "SocketChannel handleReadEvent,but wrong state " << state_;
            return;
        }
        ByteBuffer byteBuffer(kRecvBufferSize);
        SSizeType bytes = doRead(byteBuffer);
        if (bytes > 0)
        {
            channelHandlerPipeline_.fireSocketChannelRead(byteBuffer);
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
                setEvents(kWriteEvent);
                modify();
            }
        }
        else
        {
            handleReadError(errno);
        }
    }

    void SocketChannel::handleWriteEvent()
    {
        if (state_ == ChannelState::INACTIVE)
        {
            NETS_SYSTEM_LOG_WARN << "SocketChannel handleWriteEvent,but wrong state " << state_;
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
        SSizeType writtenBytes = writev(ioves, static_cast<Int32Type>(size));
        if (writtenBytes < 0)
        {
            handleWriteError(errno);
        }
        else
        {
            if (totalBytes == static_cast<SizeType>(writtenBytes))
            {
                writeBuffer_.clear();
                while (!writeCompleteCallbacks_.empty())
                {
                    WriteCompleteCallback writeCompleteCallback = writeCompleteCallbacks_.front();
                    writeCompleteCallbacks_.pop();
                    if (writeCompleteCallback != nullptr)
                    {
                        eventLoop_->addTask(
                            [this, writeCompleteCb = ::std::move(writeCompleteCallback)]()
                            {
                                writeCompleteCb(dynamic_cast<SocketChannelContext&>(channelHandlerPipeline_.context()));
                            });
                    }
                }
                if (state_ == ChannelState::HALF_CLOSE)
                {
                    channelInActive();
                }
                else
                {
                    setEvents(kReadEvent);
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
        NETS_SYSTEM_LOG_WARN << "SocketChannel handleErrorEvent,errNum=" << socket::getSockError(sockFd_);
        channelInActive();
    }

    SSizeType SocketChannel::doRead(ByteBuffer& byteBuffer)
    {
        // read all at once
        SizeType expectedReadLen = byteBuffer.writableBytes();
        SSizeType readBytes = 0;
        for (;;)
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
                Int32Type errNum = errno;
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

    void SocketChannel::doWrite(const void* data, SizeType length, WriteCompleteCallback writeCompleteCallback)
    {
        if (state_ == ChannelState::INACTIVE)
        {
            NETS_SYSTEM_LOG_ERROR << "SocketChannel write,but wrong state " << state_;
            return;
        }
        if (length == 0)
        {
            return;
        }
        // the writeBuffer has data waiting to be sent, append the data to the end of the writeBuffer
        if (!writeBuffer_.empty())
        {
            appendBuffer(data, length);
            writeCompleteCallbacks_.push(::std::move(writeCompleteCallback));
        }
        else
        {
            // write directly
            doWriteDirectly(data, length, std::move(writeCompleteCallback));
        }
    }

    void SocketChannel::doWriteDirectly(const void* data, SizeType length, WriteCompleteCallback writeCompleteCallback)
    {
        SSizeType bytes = socket::write(sockFd_, data, length);
        if (bytes < 0)
        {
            Int32Type errNum = errno;
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
            if (writeCompleteCallback != nullptr)
            {
                eventLoop_->addTask(
                    [this, writeCompleteCb = ::std::move(writeCompleteCallback)]()
                    {
                        writeCompleteCb(dynamic_cast<SocketChannelContext&>(channelHandlerPipeline_.context()));
                    });
            }
        }
        // tcp send buffer size less than length,only part of it was sent
        else
        {
            appendBuffer(static_cast<const char*>(data) + bytes, remainingBytes);
            writeCompleteCallbacks_.push(::std::move(writeCompleteCallback));
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
            addEvent(kWriteEvent);
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

    SSizeType SocketChannel::writev(const IoVecList& iovecs, Int32Type count) const
    {
        SSizeType writtenBytes = 0;
        for (Int32Type writtenBlocks = 0; writtenBlocks < count;)
        {
            Int32Type leftBlocks = count - writtenBlocks;
            SSizeType bytes = 0;
            SizeType expectedWriteLen = 0;
            // if leftBlocks less than CountOfWriteVOnce
            if (leftBlocks < kMaxCountOfWriteVOnce)
            {
                for (Int32Type i = writtenBlocks; i < leftBlocks; ++i)
                {
                    expectedWriteLen += iovecs[i].iov_len;
                }
                bytes = socket::writev(sockFd_, &iovecs[writtenBlocks], leftBlocks);
                writtenBlocks += leftBlocks;
            }
            else
            {
                for (Int32Type i = writtenBlocks; i < kMaxCountOfWriteVOnce; ++i)
                {
                    expectedWriteLen += iovecs[i].iov_len;
                }
                bytes = socket::writev(sockFd_, &iovecs[writtenBlocks], kMaxCountOfWriteVOnce);
                writtenBlocks += kMaxCountOfWriteVOnce;
            }
            if (bytes < 0)
            {
                Int32Type errNum = errno;
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
        for (auto it = writeBuffer_.begin(); it != writeBuffer_.end() && writtenBytes > 0;)
        {
            SizeType bytes = it->readableBytes();
            if (bytes <= static_cast<SizeType>(writtenBytes))
            {
                writeBuffer_.erase(it);
                writtenBytes -= bytes;
            }
            else
            {
                it->setReaderIndex(it->readerIndex() + writtenBytes);
                writtenBytes = 0;
                ++it;
            }
        }
    }

    void SocketChannel::handleReadError(Int32Type errNum)
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
                NETS_SYSTEM_LOG_ERROR << "SocketChannel handleReadError unexpected error,errno=" << errNum;
                channelInActive();
                break;
        }
    }

    void SocketChannel::handleWriteError(Int32Type errNum)
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
                NETS_SYSTEM_LOG_ERROR << "SocketChannel handleWriteError unexpected error,errno=" << errNum;
                channelInActive();
                break;
        }
    }

    void SocketChannel::channelInActive()
    {
        state_ = ChannelState::INACTIVE;
        channelHandlerPipeline_.fireSocketChannelDisconnect();
        eventLoop_->addTask(
            [channel = shared_from_this()]()
            {
                channel->deregister();
                assert(!channel->isRegistered());
                assert(channel.use_count() > 0);
            });
    }

    void SocketChannel::shutdown(Int32Type how)
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
                NETS_SYSTEM_LOG_ERROR << "SocketChannel unknown shutdown operation";
                break;
        }
    }
} // namespace nets