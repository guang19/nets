//
// Created by guang19
//

#ifndef NETS_BOOTSTRAP_H
#define NETS_BOOTSTRAP_H

#include "nets/net/bootstrap/AbstractBootstrap.h"
#include "nets/net/channel/ConnectorChannel.h"
#include "nets/net/channel/DatagramChannel.h"

namespace nets
{
    class Bootstrap : public AbstractBootstrap<Bootstrap>
    {
    public:
        using TimeType = typename ConnectorChannel::TimeType;
        using ConnectorChannelPtr = ::std::shared_ptr<ConnectorChannel>;
        using DatagramChannelPtr = ::std::shared_ptr<DatagramChannel>;
        using SocketChannelHandlerRawPtr = typename SocketChannelHandlerPipeline::SocketChannelHandlerRawPtr;
        using SocketChannelHandlerPtr = typename SocketChannelHandlerPipeline::SocketChannelHandlerPtr;
        using SocketChannelHandlerList = typename SocketChannelHandlerPipeline::SocketChannelHandlerList;
        using SocketChannelInitializationCallback = ::std::function<void(SocketChannel& channel)>;

        using DatagramChannelHandlerRawPtr = typename DatagramChannelHandlerPipeline::DatagramChannelHandlerRawPtr;
        using DatagramChannelHandlerPtr = typename DatagramChannelHandlerPipeline::DatagramChannelHandlerPtr;
        using DatagramChannelHandlerList = typename DatagramChannelHandlerPipeline::DatagramChannelHandlerList;
        using DatagramChannelInitializationCallback = ::std::function<void(DatagramChannel& channel)>;

    public:
        Bootstrap();
        ~Bootstrap() override = default;

    public:
        Bootstrap& channelHandler(SocketChannelHandlerRawPtr channelHandler);
        Bootstrap& channelHandler(const SocketChannelHandlerPtr& channelHandler);
        Bootstrap& channelHandler(const SocketChannelInitializationCallback& channelInitializationCallback);

        Bootstrap& channelHandler(DatagramChannelHandlerRawPtr channelHandler);
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
        Bootstrap& retry(bool retry = true, TimeType interval = gDefaultRetryInterval);
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
        static constexpr TimeType gDefaultRetryInterval = 2000;
    };
} // namespace nets

#endif // NETS_BOOTSTRAP_H