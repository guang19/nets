//
// Created by guang19
//

#include "nets/net/core/SocketChannel.h"

#include <thread>

#include "nets/base/log/Logging.h"
#include "nets/net/core/EventLoop.h"
#include "nets/net/exception/ChannelRegisterException.h"

namespace nets::net
{
    namespace
    {
        const SocketChannel::SizeType RecvBufferSize = DefaultTcpSockRecvBufferSize >> 1;
    }

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
        if (state_ != ChannelState::ACTIVE)
        {
            LOGS_ERROR << "SocketChannel handleReadEvent,but error state " << state_;
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
                disConnect();
            }
            else
            {
                state_ = ChannelState::HALF_CLOSE;
                shutdown(SHUT_RD);
                setEvents(EWriteEvent);
                modify();
            }
        }
        else
        {
            int32_t errNum = errno;
            handleReadError(errNum);
        }
    }

    void SocketChannel::handleWriteEvent()
    {
        if (state_ != ChannelState::ACTIVE && state_ != ChannelState::HALF_CLOSE)
        {
            LOGS_ERROR << "SocketChannel handleWriteEvent,but error state " << state_;
            return;
        }
    }

    void SocketChannel::handleErrorEvent()
    {
        int32_t errNum = socket::getSockError(sockFd_);
        LOGS_ERROR << "SocketChannel unexpected error,errNum=" << errNum;
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

    void SocketChannel::write(const StringType& message) {}

    void SocketChannel::write(const ByteBuffer& message) {}

    void SocketChannel::write(const void* message, SizeType length) {}

    void SocketChannel::shutdown()
    {
        shutdown(SHUT_RDWR);
    }

    void SocketChannel::shutdownRead()
    {
        shutdown(SHUT_RD);
    }

    void SocketChannel::shutdownWrite()
    {
        shutdown(SHUT_WR);
    }

    SSizeType SocketChannel::doRead(ByteBuffer& byteBuffer)
    {
        // read all at once
        SizeType expectedReadLen = byteBuffer.writableBytes();
        SSizeType bytes = 0;
        while (true)
        {
            bytes = byteBuffer.writeBytes(*this, expectedReadLen);
            if (bytes > 0)
            {
                // adjust read bytes for next time
                SizeType adjustment = expectedReadLen - bytes;
                if (adjustment > 0)
                {
                    expectedReadLen = adjustment;
                }
            }
            else if (bytes < 0)
            {
                int32_t errNum = errno;
                if (errNum != EAGAIN)
                {
                    errno = errNum;
                    return -1;
                }
                break;
            }
            else
            {
                return 0;
            }
        }
        return static_cast<SSizeType>(byteBuffer.readableBytes());
    }

    void SocketChannel::doWrite(const void* data, SizeType length)
    {
        if (length == 0)
        {
            return;
        }
        if (state_ != ChannelState::ACTIVE && state_ != ChannelState::HALF_CLOSE)
        {
            LOGS_ERROR << "SocketChannel write,but error state " << state_;
            return;
        }
        // the writeBuffer has residual data waiting to be sent, append the data to the end of the writeBuffer
        if (!writeBuffer_.empty())
        {
            appendBuffer(data, length);
            return;
        }
        // write directly
        doWriteDirectly(data, length);
    }

    void SocketChannel::doWriteDirectly(const void* data, SizeType length)
    {
        SSizeType bytes = socket::write(sockFd_, data, length);
        if (bytes < 0)
        {
            int32_t errNum = errno;
            // ignore write EAGAIN
            if (errNum == EAGAIN || errNum == EINTR)
            {
                bytes = 0;
            }
            else
            {
                errno = errNum;
                handleErrorEvent();
            }
        }
        SizeType remaining = length - bytes;
        // write complete
        if (remaining == 0)
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
        // tcp send buffer size less than length,only part of it is sent
        else
        {
            appendBuffer(static_cast<const char*>(data) + bytes, remaining);
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

    void SocketChannel::handleReadError(int32_t errNum)
    {
        switch (errNum)
        {
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
                break;
        }
    }

    void SocketChannel::disConnect()
    {
        state_ = ChannelState::INACTIVE;
        shutdown(SHUT_RDWR);
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

    void SocketChannel::shutdown(int32_t how)
    {
        socket::shutdown(sockFd_, how);
    }
} // namespace nets::net