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

// @brief The encapsulation of tcp client operations

#include "nets/net/channel/ConnectorChannel.h"

#include <cassert>

#include "nets/base/log/Logger.h"
#include "nets/net/core/EventLoop.h"
#include "nets/net/exception/ChannelRegisterException.h"

namespace nets
{
    ConnectorChannel::ConnectorChannel(EventLoopRawPtr eventLoop)
        : Channel(eventLoop), sockFd_(socket::kInvalidFd), localAddress_(), peerAddress_(),
          state_(ConnectionState::DISCONNECTED), retry_(false), retryInterval_(0), channelOptions_(), channelHandlers_(),
          channelInitializationCallback_()
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
            NETS_SYSTEM_LOG_WARN << "ConnectorChannel handleWriteEvent connectionState=" << state_;
            return;
        }
        Int32Type errNum = socket::getSockError(sockFd_);
        if (errNum != 0)
        {
            NETS_SYSTEM_LOG_ERROR << "ConnectorChannel handleWriteEvent unable to connect to " << peerAddress_.toString()
                                  << ",errNum=" << errNum;
            handleErrorEvent();
        }
        else
        {
            newSocketChannel();
        }
    }

    void ConnectorChannel::handleErrorEvent()
    {
        assert(state_ != ConnectionState::CONNECTED);
        if (retry_)
        {
            eventLoop_->addTask(
                [self = ::std::dynamic_pointer_cast<ConnectorChannel>(shared_from_this())]()
                {
                    assert(self->isRegistered());
                    self->deregister();
                    assert(!self->isRegistered());
                    assert(self.use_count() == 1);
                    self->reconnect();
                });
        }
        else
        {
            // wait next loop deregister, because channel might has other event
            eventLoop_->addTask(
                [channel = shared_from_this()]()
                {
                    assert(channel->isRegistered());
                    channel->deregister();
                    assert(!channel->isRegistered());
                    assert(channel.use_count() == 1);
                });
        }
    }

    void ConnectorChannel::connect(const InetSockAddress& serverAddress)
    {
        sockFd_ = socket::createTcpSocket(serverAddress.ipFamily());
        socket::setSockNonBlock(sockFd_, true);
        Int32Type ret = socket::connect(sockFd_, serverAddress);
        peerAddress_ = serverAddress;
        assert(state_ == ConnectionState::DISCONNECTED);
        if (ret == 0)
        {
            newSocketChannel();
        }
        else
        {
            handleConnectError(errno);
        }
    }

    void ConnectorChannel::initSocketChannel(SocketChannelPtr& socketChannel)
    {
        if (!channelOptions_.empty())
        {
            socketChannel->setChannelOptions(channelOptions_);
            channelOptions_.clear();
        }
        if (!channelHandlers_.empty())
        {
            for (const auto& channelHandler : channelHandlers_)
            {
                assert(channelHandler.use_count() == 1);
                socketChannel->pipeline().addLast(channelHandler);
            }
            channelHandlers_.clear();
        }
        if (channelInitializationCallback_ != nullptr)
        {
            channelInitializationCallback_(*socketChannel);
            channelInitializationCallback_ = nullptr;
        }
        assert(channelOptions_.empty());
        assert(channelHandlers_.empty());
        assert(channelInitializationCallback_ == nullptr);
    }

    void ConnectorChannel::newSocketChannel()
    {
        assert(state_ != ConnectionState::CONNECTED);
        socket::getLocalAddress(sockFd_, localAddress_);
        auto socketChannel = ::std::make_shared<SocketChannel>(sockFd_, localAddress_, peerAddress_, eventLoop_);
        initSocketChannel(socketChannel);
        // must remove self from EventLoop before socketChannel register
        if (state_ == ConnectionState::CONNECTING)
        {
            state_ = ConnectionState::CONNECTED;
            assert(isRegistered());
            deregister();
            assert(!isRegistered());
        }
        else
        {
            state_ = ConnectionState::CONNECTED;
        }
        socketChannel->channelActive();
    }

    void ConnectorChannel::handleConnectError(Int32Type errNum)
    {
        switch (errNum)
        {
            // my not error, need to use epoll to check the connection
            case EINTR:
            case EISCONN:
            case EINPROGRESS:
            {
                waitConnect();
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
                NETS_SYSTEM_LOG_ERROR << "ConnectorChannel occurred unexpected exception while connecting,errno=" << errNum;
                break;
        }
    }

    void ConnectorChannel::waitConnect()
    {
        addEvent(kWriteEvent);
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
            NETS_SYSTEM_LOG_ERROR << "ConnectorChannel waitConnect failed,cause " << exception.what();
            if (retry_)
            {
                reconnect();
            }
        }
    }

    void ConnectorChannel::reconnect()
    {
        NETS_SYSTEM_LOG_WARN << "ConnectorChannel reconnect";
        assert(state_ != ConnectionState::CONNECTED);
        state_ = ConnectionState::DISCONNECTED;
        socket::closeFd(sockFd_);
        sockFd_ = socket::kInvalidFd;
        // schedule reconnect
        eventLoop_->schedule(retryInterval_,
                             [self = ::std::dynamic_pointer_cast<ConnectorChannel>(shared_from_this())]()
                             {
                                 self->connect(self->peerAddress_);
                             });
    }
} // namespace nets
