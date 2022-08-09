//
// Created by guang19 on 2022/5/18.
//

#ifndef NETS_NET_BOOTSTRAP_H
#define NETS_NET_BOOTSTRAP_H

#include "nets/net/core/AbstractBootstrap.h"
#include "nets/net/core/SocketChannel.h"

namespace nets::net
{
    class Bootstrap : public AbstractBootstrap<Bootstrap>
    {
    public:
        using ChannelHandlerRawPtr = ChannelHandlerPipeline::ChannelHandlerRawPtr;
        using ChannelHandlerPtr = ChannelHandlerPipeline::ChannelHandlerPtr;
        using ChannelHandlerList = ChannelHandlerPipeline::ChannelHandlerList;
        using ChannelInitializationCallback = ::std::function<void(SocketChannel& channel)>;

    public:
        Bootstrap();
        ~Bootstrap() = default;

    public:
        Bootstrap& handler(ChannelHandlerRawPtr channelHandler);
        Bootstrap& handler(const ChannelHandlerPtr& channelHandler);
        Bootstrap& handler(const ChannelInitializationCallback& channelInitializationCallback);

        Bootstrap& connect(const char* ip, PortType port, bool ipv6 = false);
        Bootstrap& connect(const InetSockAddress& serverAddress);

    private:
        void doConnect(const InetSockAddress& serverAddress);

    private:
        ChannelHandlerList channelHandlers_ {};
        ChannelInitializationCallback channelInitializationCallback_ {};
    };
} // namespace nets::net

#endif // NETS_NET_BOOTSTRAP_H
