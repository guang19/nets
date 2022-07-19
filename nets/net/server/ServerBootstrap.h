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
        using NType = typename EventLoopGroup::NType;
        using EventLoopGroupPtr = ::std::unique_ptr<EventLoopGroup>;
        using ServerSocketChannelPtr = ::std::shared_ptr<ServerSocketChannel>;
        using ChannelHandlerPtr = typename ServerSocketChannel::ChannelHandlerPtr;
        using ChannelInitializationCallback = typename ServerSocketChannel::ChannelInitializationCallback;

    public:
        explicit ServerBootstrap(NType numOfSubEventLoops);
        explicit ServerBootstrap(NType numOfMainEventLoops, NType numOfSubEventLoops);
        ~ServerBootstrap();

    public:
        ServerBootstrap& channelHandler(ChannelHandlerPtr channelHandler);
        ServerBootstrap& channelHandler(const ChannelInitializationCallback& channelInitializationCallback);
        ServerBootstrap& bind(PortType port);
        ServerBootstrap& bind(const char* ip, PortType port);
        ServerBootstrap& bind(const InetSockAddress& localAddress);
        void launch();

    private:
        ::std::atomic_bool running_ {false};
        ServerSocketChannelPtr serverSocketChannel_ {nullptr};
        EventLoopGroupPtr mainLoopGroup_ {nullptr};
        EventLoopGroupPtr subLoopGroup_ {nullptr};
    };
} // namespace nets::net

#endif // NETS_NET_SERVER_BOOTSTRAP_H
