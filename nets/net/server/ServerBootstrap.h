//
// Created by guang19 on 2022/5/18.
//

#ifndef NETS_NET_SERVER_BOOTSTRAP_H
#define NETS_NET_SERVER_BOOTSTRAP_H

#include "nets/net/core/ChannelHandler.h"
#include "nets/net/core/EventLoopGroup.h"
#include "nets/net/server/ServerSocketChannel.h"

namespace nets::net
{
    class ServerBootstrap : nets::base::Noncopyable
    {
    public:
        using NType = EventLoopGroup::NType;
        using EventLoopGroupPtr = ::std::unique_ptr<EventLoopGroup>;
        using ChannelHandlerRawPtr = ChannelHandler*;
        using ChannelInitializationCallback = ServerSocketChannel::ChannelInitializationCallback;

    public:
        ServerBootstrap(NType numOfSubEventLoops);
        ServerBootstrap(NType numOfMainEventLoops, NType numOfSubEventLoops);
        ~ServerBootstrap();

    public:
        ServerBootstrap& channelHandler(const ChannelInitializationCallback& channelInitializationCallback);
        ServerBootstrap& bind(PortType port);
        ServerBootstrap& bind(const char* ip, PortType port);
        ServerBootstrap& bind(const InetSockAddress& localAddress);
        void launch();

    private:
        ::std::atomic_bool running_ {false};
        ChannelInitializationCallback channelInitializationCallback_ {nullptr};
        EventLoopGroupPtr mainLoopGroup_ {nullptr};
        EventLoopGroupPtr subLoopGroup_ {nullptr};
    };
} // namespace nets::net

#endif // NETS_NET_SERVER_BOOTSTRAP_H
