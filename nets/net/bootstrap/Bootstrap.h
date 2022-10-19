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

#ifndef NETS_BOOTSTRAP_H
#define NETS_BOOTSTRAP_H

#include "nets/net/bootstrap/AbstractBootstrap.h"
#include "nets/net/channel/ConnectorChannel.h"
#include "nets/net/channel/DatagramChannel.h"
#include "nets/net/core/DatagramChannelHandler.h"

namespace nets
{
    class Bootstrap : public AbstractBootstrap<Bootstrap>
    {
    private:
        using ConnectorChannelPtr = ::std::shared_ptr<ConnectorChannel>;
        using DatagramChannelPtr = ::std::shared_ptr<DatagramChannel>;
        using SocketChannelHandlerPtr = typename ConnectorChannel::ChannelHandlerPtr;
        using SocketChannelHandlerList = typename ConnectorChannel::ChannelHandlerList;
        using SocketChannelInitializationCallback = ::std::function<void(SocketChannel& channel)>;
        using DatagramChannelHandlerPtr = typename DatagramChannelHandler::DatagramChannelHandlerPtr;
        using DatagramChannelHandlerList = ::std::list<DatagramChannelHandlerPtr>;
        using DatagramChannelInitializationCallback = ::std::function<void(DatagramChannel& channel)>;

    public:
        Bootstrap();
        ~Bootstrap() override = default;

    public:
        Bootstrap& channelHandler(const SocketChannelHandlerPtr& channelHandler);
        Bootstrap& channelHandler(const SocketChannelInitializationCallback& channelInitializationCallback);

        Bootstrap& channelHandler(const DatagramChannelHandlerPtr& channelHandler);
        Bootstrap& channelHandler(const DatagramChannelInitializationCallback& channelInitializationCallback);

        // udp does not bind addr
        Bootstrap& bind();
        // udp bind addr
        Bootstrap& bind(PortType port, bool ipv6 = false);
        Bootstrap& bind(const StringType& ip, PortType port, bool ipv6 = false);
        Bootstrap& bind(const InetSockAddress& localAddress);

        // whether to retry after connect failure
        // interval is reconnect interval, unit:milliseconds
        // when retry is true, the interval takes effect
        Bootstrap& retry(bool retry = true, TimeType interval = kDefaultRetryInterval);
        // tcp client
        Bootstrap& connect(const StringType& ip, PortType port, bool ipv6 = false);
        Bootstrap& connect(const InetSockAddress& serverAddress);

        void sync();

    private:
        void doConnect(const InetSockAddress& serverAddress);
        void initConnectorChannel(ConnectorChannelPtr& connectorChannel);
        void doBind(const InetSockAddress& localAddress);
        void initDatagramChannel(DatagramChannelPtr& datagramChannel);

    private:
        bool retry_;
        TimeType retryInterval_;
        SocketChannelHandlerList socketChannelHandlers_;
        DatagramChannelHandlerList datagramChannelHandlers_;
        SocketChannelInitializationCallback socketChannelInitializationCallback_;
        DatagramChannelInitializationCallback datagramChannelInitializationCallback_;

        // default retry interval
        static constexpr TimeType kDefaultRetryInterval = 2000;
    };
} // namespace nets

#endif // NETS_BOOTSTRAP_H