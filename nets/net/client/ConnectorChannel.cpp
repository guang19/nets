//
// Created by guang19 on 2022/8/10.
//

#include "nets/net/client/ConnectorChannel.h"

#include <cassert>

#include "nets/base/CommonMacro.h"
#include "nets/base/log/Logging.h"
#include "nets/net/core/EventLoop.h"
#include "nets/net/exception/ChannelRegisterException.h"

namespace nets::net
{
    ConnectorChannel::ConnectorChannel(EventLoopRawPtr eventLoop)
        : Channel(eventLoop), sockFd_(socket::InvalidFd), localAddress_(), peerAddress_(),
          connectionState_(ConnectionState::DISCONNECTED)
    {
    }

    ConnectorChannel::~ConnectorChannel()
    {
        // prevent close of connected fd
        if (connectionState_ != ConnectionState::CONNECTED)
        {
            socket::closeFd(sockFd_);
        }
    }

    FdType ConnectorChannel::fd() const
    {
        return sockFd_;
    }

    void ConnectorChannel::handleWriteEvent()
    {
        assert(connectionState_ == ConnectionState::CONNECTING);
        if (connectionState_ != ConnectionState::CONNECTING)
        {
            return;
        }
        int32_t errNum = socket::getSockError(sockFd_);
        if (errNum != 0)
        {
            LOGS_ERROR << "ConnectorChannel occurred unexpected exception while connecting,errNum=" << errNum;
            reconnect();
            return;
        }
        if (socket::isSelfConnect(sockFd_))
        {
            LOGS_ERROR << "ConnectorChannel occurred unexpected exception while connecting,errNum=" << errNum;
            return;
        }
        connectionState_ = ConnectionState::CONNECTED;
        auto socketChannel = ::std::make_shared<SocketChannel>(sockFd_, localAddress_, peerAddress_, eventLoop_);
        initSocketChannel(socketChannel);
        // must remove self before socketChannel register
        deregister();
        socketChannel->channelActive();
    }

    void ConnectorChannel::handleErrorEvent()
    {
        assert(connectionState_ != ConnectionState::CONNECTED);
    }

    void ConnectorChannel::connect(const InetSockAddress& serverAddress)
    {
        sockFd_ = socket::createTcpSocket(serverAddress.ipFamily());
        socket::setSockNonBlock(sockFd_);
        int32_t ret = socket::connect(sockFd_, serverAddress.sockAddr());
        peerAddress_ = serverAddress;
        if (ret == 0)
        {
            channelActive();
        }
        else
        {
            int32_t errNum = errno;
            handleConnectError(errNum);
        }
    }

    void ConnectorChannel::channelActive()
    {
        socket::getLocalAddress(sockFd_, localAddress_.sockAddr6());
        addEvent(EWriteEvent);
        try
        {
            if (!registerTo())
            {
                THROW_FMT(ChannelRegisterException, "ConnectorChannel register failed");
            }
            connectionState_ = ConnectionState::CONNECTING;
        }
        catch (const ChannelRegisterException& exception)
        {
            if (isRegistered())
            {
                deregister();
            }
        }
    }

    void ConnectorChannel::initSocketChannel(SocketChannelPtr& socketChannel)
    {
        socketChannel->setChannelOptions(channelOptions_);
        for (const auto& channelHandler: channelHandlers_)
        {
            assert(channelHandler.use_count() == 1);
            socketChannel->pipeline().addLast(channelHandler);
        }
        if (channelInitializationCallback_ != nullptr)
        {
            channelInitializationCallback_(*socketChannel);
        }
    }

    void ConnectorChannel::handleConnectError(int32_t errNum)
    {
        switch (errNum)
        {
            // my not error, need to use epoll to check the connection
            case EINTR:
            case EISCONN:
            case EINPROGRESS:
            {
                channelActive();
                break;
            }
            // retry
            case EAGAIN:
            case ETIMEDOUT:
            case EADDRINUSE:
            case EADDRNOTAVAIL:
            case ECONNREFUSED:
            case ENETUNREACH:
            {
                reconnect();
                break;
            }
            case EACCES:
            case EPERM:
            case EAFNOSUPPORT:
            case EALREADY:
            case EBADF:
            case EFAULT:
            case ENOTSOCK:
            case EPROTOTYPE:
            default:
                errno = errNum;
                LOGS_ERROR << "ConnectorChannel connect unexpected exception,errno=" << errNum;
                socket::closeFd(sockFd_);
                break;
        }
    }

    void ConnectorChannel::reconnect()
    {
        LOGS_WARN << "ConnectorChannel reconnect";
        connectionState_ = ConnectorChannel::DISCONNECTED;
        socket::closeFd(sockFd_);
        // schedule reconnect
    }
} // namespace nets::net
