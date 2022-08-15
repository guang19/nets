//
// Created by guang19
//

#ifndef NETS_NET_BOOTSTRAP_H
#define NETS_NET_BOOTSTRAP_H

#include "nets/net/client/ConnectorChannel.h"
#include "nets/net/core/AbstractBootstrap.h"

namespace nets::net
{
    class Bootstrap : public AbstractBootstrap<Bootstrap>
    {
    public:
        using ConnectorChannelPtr = ::std::shared_ptr<ConnectorChannel>;
        using ChannelHandlerRawPtr = ChannelHandlerPipeline::ChannelHandlerRawPtr;
        using ChannelHandlerPtr = ChannelHandlerPipeline::ChannelHandlerPtr;
        using ChannelHandlerList = ChannelHandlerPipeline::ChannelHandlerList;
        using ChannelInitializationCallback = ::std::function<void(SocketChannel& channel)>;

    public:
        Bootstrap();
        ~Bootstrap() = default;

    public:
        Bootstrap& channelHandler(ChannelHandlerRawPtr channelHandler);
        Bootstrap& channelHandler(const ChannelHandlerPtr& channelHandler);
        Bootstrap& channelHandler(const ChannelInitializationCallback& channelInitializationCallback);

        Bootstrap& connect(const char* ip, PortType port, bool ipv6 = false);
        Bootstrap& connect(const InetSockAddress& serverAddress);

        void sync();

    private:
        void doConnect(const InetSockAddress& serverAddress);
        void initConnectorChannel(::std::shared_ptr<ConnectorChannel>& connectorChannel);

    private:
        ChannelHandlerList channelHandlers_ {};
        ChannelInitializationCallback channelInitializationCallback_ {};
    };
} // namespace nets::net

#endif // NETS_NET_BOOTSTRAP_H