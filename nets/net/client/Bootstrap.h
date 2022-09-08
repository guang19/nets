//
// Created by guang19
//

#ifndef NETS_NET_BOOTSTRAP_H
#define NETS_NET_BOOTSTRAP_H

#include "nets/net/client/ConnectorChannel.h"
#include "nets/net/core/AbstractBootstrap.h"
#include "nets/net/core/DatagramChannel.h"

namespace nets::net
{
    class Bootstrap : public AbstractBootstrap<Bootstrap>
    {
    public:
        using TimeType = typename ConnectorChannel::TimeType;
        using ConnectorChannelPtr = ::std::shared_ptr<ConnectorChannel>;
        using DatagramChannelPtr = ::std::shared_ptr<DatagramChannel>;
        using SocketChannelHandlerRawPtr = typename ChannelHandlerPipeline::SocketChannelHandlerRawPtr;
        using SocketChannelHandlerPtr = typename ChannelHandlerPipeline::SocketChannelHandlerPtr;
        using ChannelHandlerRawPtr = typename ChannelHandlerPipeline::ChannelHandlerRawPtr;
        using ChannelHandlerPtr = typename ChannelHandlerPipeline::ChannelHandlerPtr;
        using ChannelHandlerList = typename ChannelHandlerPipeline::ChannelHandlerList;
        using ChannelInitializationCallback = ::std::function<void(SocketChannel& channel)>;

    public:
        Bootstrap();
        ~Bootstrap() override = default;

    public:
        Bootstrap& channelHandler(ChannelHandlerRawPtr channelHandler);
        Bootstrap& channelHandler(const ChannelHandlerPtr& channelHandler);
        Bootstrap& channelHandler(const ChannelInitializationCallback& channelInitializationCallback);

        // udp client
        Bootstrap& bind();
        // udp server
        Bootstrap& bind(const char* ip, PortType port, bool ipv6 = false);
        Bootstrap& bind(const InetSockAddress& localAddress);

        // whether to retry after connect failure
        // interval is reconnect interval, unit:milliseconds
        // when retry is true, the interval takes effect
        Bootstrap& retry(bool retry = true, TimeType interval = DefaultRetryInterval);
        // tcp client
        Bootstrap& connect(const char* ip, PortType port, bool ipv6 = false);
        Bootstrap& connect(const InetSockAddress& serverAddress);

        void sync();

    private:
        void doConnect(const InetSockAddress& serverAddress);
        void initConnectorChannel(ConnectorChannelPtr& connectorChannel);
        void doBind(const InetSockAddress& localAddress);
        void initDatagramChannel(DatagramChannelPtr& datagramChannel);

    private:
        bool retry_ {false};
        TimeType retryInterval_ {0};
        ChannelHandlerList channelHandlers_ {};
        ChannelInitializationCallback channelInitializationCallback_ {};

        // default retry interval
        static constexpr TimeType DefaultRetryInterval = 2000;
    };
} // namespace nets::net

#endif // NETS_NET_BOOTSTRAP_H