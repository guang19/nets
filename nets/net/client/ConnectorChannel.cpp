//
// Created by guang19
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
          state_(ConnectionState::DISCONNECTED)
    {
    }

    ConnectorChannel::~ConnectorChannel()
    {
        // prevent close of connected fd
        if (state_ != ConnectionState::CONNECTED)
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
        if (state_ != ConnectionState::CONNECTING)
        {
            LOGS_WARN << "ConnectorChannel handleWriteEvent connectionState=" << state_;
            return;
        }
        int32_t errNum = socket::getSockError(sockFd_);
        if (errNum != 0)
        {
            LOGS_ERROR << "ConnectorChannel handleWriteEvent unable to connect to " << peerAddress_.toString()
                       << ",errNum=" << errNum;
            handleErrorEvent();
            return;
        }
        channelActive();
    }

    void ConnectorChannel::handleErrorEvent()
    {
        LOGS_ERROR << "ConnectorChannel handleErrorEvent unable to connect to " << peerAddress_.toString();
        assert(state_ != ConnectionState::CONNECTED);
        if (state_ == ConnectionState::CONNECTING)
        {
            // wait next loop deregister, because channel might has other event
            eventLoop_->addTask(
                [channel = shared_from_this()]()
                {
                    channel->deregister();
                    assert(!channel->isRegistered());
                    assert(channel.use_count() == 1);
                });
        }
    }

    void ConnectorChannel::connect(const InetSockAddress& serverAddress)
    {
        sockFd_ = socket::createTcpSocket(serverAddress.ipFamily());
        socket::setSockNonBlock(sockFd_);
        int32_t ret = socket::connect(sockFd_, serverAddress.sockAddr());
        peerAddress_ = serverAddress;
        if (ret == 0)
        {
            socket::getLocalAddress(sockFd_, localAddress_.sockAddr6());
            assert(state_ == ConnectionState::DISCONNECTED);
            state_ = ConnectionState::CONNECTED;
            channelActive();
        }
        else
        {
            int32_t errNum = errno;
            handleConnectError(errNum);
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

    void ConnectorChannel::channelActive()
    {
        auto socketChannel = ::std::make_shared<SocketChannel>(sockFd_, localAddress_, peerAddress_, eventLoop_);
        initSocketChannel(socketChannel);
        // must remove self from EventLoop before socketChannel register
        if (state_ == ConnectionState::CONNECTING)
        {
            state_ = ConnectionState::CONNECTED;
            deregister();
            assert(!isRegistered());
        }
        socketChannel->channelActive();
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
                checkConnected();
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
                LOGS_ERROR << "ConnectorChannel occurred unexpected exception occurred while connecting,errno=" << errNum;
                break;
        }
    }

    void ConnectorChannel::checkConnected()
    {
        socket::getLocalAddress(sockFd_, localAddress_.sockAddr6());
        addEvent(EWriteEvent);
        try
        {
            if (!registerTo())
            {
                THROW_FMT(ChannelRegisterException, "ConnectorChannel register failed");
            }
            state_ = ConnectionState::CONNECTING;
        }
        catch (const ChannelRegisterException& exception)
        {
            if (isRegistered())
            {
                deregister();
            }
            LOGS_ERROR << "ConnectorChannel checkConnected failed,cause " << exception.what();
            reconnect();
        }
    }

    void ConnectorChannel::reconnect()
    {
        LOGS_WARN << "ConnectorChannel reconnect";
        state_ = ConnectorChannel::DISCONNECTED;
        socket::closeFd(sockFd_);
        // schedule reconnect
    }
} // namespace nets::net
