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

// @brief Bootstrap for tcp client and udp(server or client)

#include "nets/net/bootstrap/Bootstrap.h"

namespace nets
{
    Bootstrap::Bootstrap()
        : AbstractBootstrap(), retry_(false), retryInterval_(0), socketChannelHandlers_(), datagramChannelHandlers_(),
          socketChannelInitializationCallback_(), datagramChannelInitializationCallback_()
    {
    }

    Bootstrap& Bootstrap::channelHandler(SocketChannelHandlerRawPtr channelHandler)
    {
        socketChannelHandlers_.push_back(SocketChannelHandlerPtr(channelHandler));
        return *this;
    }

    Bootstrap& Bootstrap::channelHandler(const SocketChannelHandlerPtr& channelHandler)
    {
        socketChannelHandlers_.push_back(channelHandler);
        return *this;
    }

    Bootstrap& Bootstrap::channelHandler(const SocketChannelInitializationCallback& channelInitializationCallback)
    {
        socketChannelInitializationCallback_ = channelInitializationCallback;
        return *this;
    }

    Bootstrap& Bootstrap::channelHandler(DatagramChannelHandlerRawPtr channelHandler)
    {
        datagramChannelHandlers_.push_back(DatagramChannelHandlerPtr(channelHandler));
        return *this;
    }

    Bootstrap& Bootstrap::channelHandler(const DatagramChannelHandlerPtr& channelHandler)
    {
        datagramChannelHandlers_.push_back(channelHandler);
        return *this;
    }

    Bootstrap& Bootstrap::channelHandler(const DatagramChannelInitializationCallback& channelInitializationCallback)
    {
        datagramChannelInitializationCallback_ = channelInitializationCallback;
        return *this;
    }

    Bootstrap& Bootstrap::bind()
    {
        return bind(InetSockAddress());
    }

    Bootstrap& Bootstrap::bind(PortType port, bool ipv6)
    {
        return bind(InetSockAddress::createAnySockAddress(port, ipv6));
    }

    Bootstrap& Bootstrap::bind(const StringType& ip, PortType port, bool ipv6)
    {
        return bind(InetSockAddress(ip, port, ipv6));
    }

    Bootstrap& Bootstrap::bind(const InetSockAddress& localAddress)
    {
        mainLoopGroup_.loopEach();
        auto future = mainLoopGroup_.submit(
            [this, &localAddress]()
            {
                doBind(localAddress);
            });
        future.wait();
        return *this;
    }

    Bootstrap& Bootstrap::retry(bool retry, TimeType interval)
    {
        retry_ = retry;
        retryInterval_ = interval;
        return *this;
    }

    Bootstrap& Bootstrap::connect(const StringType& ip, PortType port, bool ipv6)
    {
        connect(InetSockAddress(ip, port, ipv6));
        return *this;
    }

    Bootstrap& Bootstrap::connect(const InetSockAddress& serverAddress)
    {
        mainLoopGroup_.loopEach();
        auto future = mainLoopGroup_.submit(
            [this, &serverAddress]()
            {
                doConnect(serverAddress);
            });
        future.wait();
        return *this;
    }

    void Bootstrap::sync()
    {
        mainLoopGroup_.syncEach();
    }

    void Bootstrap::doConnect(const InetSockAddress& serverAddress)
    {
        auto connectorChannel = ::std::make_shared<ConnectorChannel>(mainLoopGroup_.next());
        initConnectorChannel(connectorChannel);
        connectorChannel->connect(serverAddress);
    }

    void Bootstrap::initConnectorChannel(ConnectorChannelPtr& connectorChannel)
    {
        connectorChannel->setRetry(retry_);
        connectorChannel->setRetryInterval(retryInterval_);

        if (!channelOptions_.empty())
        {
            connectorChannel->setChannelOptions(channelOptions_);
            channelOptions_.clear();
        }

        if (!socketChannelHandlers_.empty())
        {
            connectorChannel->setChannelHandlers(socketChannelHandlers_);
            socketChannelHandlers_.clear();
        }

        if (socketChannelInitializationCallback_ != nullptr)
        {
            connectorChannel->setChannelInitializationCallback(socketChannelInitializationCallback_);
            socketChannelInitializationCallback_ = nullptr;
        }

        assert(channelOptions_.empty());
        assert(socketChannelHandlers_.empty());
        assert(socketChannelInitializationCallback_ == nullptr);
    }

    void Bootstrap::doBind(const InetSockAddress& localAddress)
    {
        auto datagramChannel = ::std::make_shared<DatagramChannel>(mainLoopGroup_.next());
        initDatagramChannel(datagramChannel);
        datagramChannel->bind(localAddress);
    }

    void Bootstrap::initDatagramChannel(DatagramChannelPtr& datagramChannel)
    {
        if (!channelOptions_.empty())
        {
            datagramChannel->setChannelOptions(channelOptions_);
            channelOptions_.clear();
        }

        if (!datagramChannelHandlers_.empty())
        {
            for (const auto& childHandler : datagramChannelHandlers_)
            {
                assert(childHandler.use_count() == 1);
                datagramChannel->pipeline().addLast(childHandler);
            }
            datagramChannelHandlers_.clear();
        }

        if (datagramChannelInitializationCallback_ != nullptr)
        {
            datagramChannelInitializationCallback_(*datagramChannel);
            datagramChannelInitializationCallback_ = nullptr;
        }
        assert(channelOptions_.empty());
        assert(datagramChannelHandlers_.empty());
        assert(datagramChannelInitializationCallback_ == nullptr);
    }
} // namespace nets