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
        Bootstrap& channelHandler(ChannelHandlerRawPtr channelHandler);
        Bootstrap& channelHandler(const ChannelHandlerPtr& channelHandler);
        Bootstrap& channelHandler(const ChannelInitializationCallback& channelInitializationCallback);

        Bootstrap& connect(const char* ip, PortType port, bool ipv6 = false);
        Bootstrap& connect(const InetSockAddress& serverAddress);

        void sync();

    private:
        void doConnect(const InetSockAddress& serverAddress);
        void initSocketChannel(::std::shared_ptr<SocketChannel>& socketChannel);

    private:
        ChannelHandlerList channelHandlers_ {};
        ChannelInitializationCallback channelInitializationCallback_ {};
    };
} // namespace nets::net

#endif // NETS_NET_BOOTSTRAP_H
