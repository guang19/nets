//
// Created by guang19 on 2022/5/19.
//

#include "nets/net/core/SocketChannel.h"

#include <thread>

#include "nets/base/log/Logging.h"
#include "nets/net/core/EventLoop.h"
#include "nets/net/exception/ChannelRegisterException.h"

namespace nets::net
{
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
            LOGS_ERROR << "SocketChannel handleReadEvent error state " << state_;
            return;
        }
        ByteBuffer byteBuffer(DefaultTcpSockRecvBufSize);
        SSizeType bytes = byteBuffer.writeBytes(*this);
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
            // the peer may shutdown write or close socket, the local can not close socket directly.
            // if the peer shutdown write, but the local can still send data, the peer can also read the data send by local.
            // So if the local still has data waiting to be sent, just shutdown read; if there is no data waiting to be sent,
            // shutdown both
            if (writeBuffer_.empty())
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
            LOGS_ERROR << "SocketChannel handleWriteEvent error state " << state_;
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

    void SocketChannel::write(const void* message, IntType len) {}

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

    void SocketChannel::handleReadError(int32_t errNum)
    {
        switch (errNum)
        {
            // EAGAIN/EWOULDBLOCK is not an error
            // EWOULDBLOCK
            case EAGAIN:
            case EINTR:
                LOGS_ERROR << "SocketChannel handleReadError errno=" << errNum;
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
                break;
        }
    }

    void SocketChannel::shutdown(int32_t how)
    {
        socket::shutdown(sockFd_, how);
    }
} // namespace nets::net