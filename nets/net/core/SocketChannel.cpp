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
        : Channel(eventLoop), sockFd_(sockFd), localAddress_(localAddress), peerAddress_(peerAddress), sendBuffer_(),
          state_(ChannelState::INACTIVE), channelHandlerPipeline_(this)
    {
    }

    SocketChannel::~SocketChannel()
    {
        socket::closeFd(sockFd_);
    }

    void SocketChannel::init()
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
        }
        catch (const ::std::exception& exception)
        {
            channelHandlerPipeline_.fireExceptionCaught(exception);
        }
    }

    void SocketChannel::setChannelOptions(const ChannelOptionList& channelOptions)
    {
        for (const auto& channelOption: channelOptions)
        {
            setChannelOption(channelOption);
        }
    }

    void SocketChannel::connect()
    {

    }

    void SocketChannel::write(const StringType& message)
    {

    }

    void SocketChannel::write(const void* message, IntType len)
    {

    }

    void SocketChannel::write(const ByteBuffer& message)
    {

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
            socket::shutdown(sockFd_, SHUT_RD);
            state_ = ChannelState::HALF_CLOSE;
        }
        else
        {
            int32_t errNum = errno;
            handleReadError(errNum);
        }
    }

    void SocketChannel::handleReadError(int32_t errNum)
    {
        switch (errNum)
        {
            // EAGAIN/EWOULDBLOCK is not an error
            // EWOULDBLOCK
            case EAGAIN:
            case EINTR:
                LOGS_ERROR << "SocketChannel read expected exception,errno=" << errNum;
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
                errno = errNum;
                LOGS_ERROR << "SocketChannel read unexpected exception,errno=" << errNum;
                break;
        }
    }

    void SocketChannel::handleWriteEvent()
    {
        if (state_ != ChannelState::ACTIVE && state_ != ChannelState::HALF_CLOSE)
        {
            LOGS_ERROR << "SocketChannel handleReadEvent error state " << state_;
            return;
        }
    }

    void SocketChannel::handleErrorEvent()
    {
        int32_t errNum = socket::getSockError(sockFd_);
        LOGS_ERROR << "SocketChannel unexpected exception,errNum=" << errNum;
    }
} // namespace nets::net