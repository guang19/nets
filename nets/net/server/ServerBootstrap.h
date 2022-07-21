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
        using ChannelHandlerPtr = typename ServerSocketChannel::ChannelHandlerPtr;
        using ChannelHandlerList = typename ServerSocketChannel::ChannelHandlerList;
        using ChannelInitializationCallback = typename ServerSocketChannel::ChannelInitializationCallback;

    public:
        explicit ServerBootstrap(NType numOfSubEventLoops);
        explicit ServerBootstrap(NType numOfMainEventLoops, NType numOfSubEventLoops);
        ~ServerBootstrap();

    public:
        // set the ChannelHandler shared by all channels
        ServerBootstrap& channelHandler(const ChannelHandlerPtr& channelHandler);
        // set the ChannelHandler for each channel
        ServerBootstrap& channelHandler(const ChannelInitializationCallback& channelInitializationCallback);
        ServerBootstrap& bind(PortType port);
        ServerBootstrap& bind(const char* ip, PortType port);
        ServerBootstrap& bind(const InetSockAddress& localAddress);
        void launch();

    private:
        void doBind(const InetSockAddress& localAddress);

    private:
        ::std::atomic_bool running_ {false};
        ChannelHandlerList channelHandlers_ {};
        ChannelInitializationCallback channelInitializationCallback_ {};
        EventLoopGroupPtr mainLoopGroup_ {nullptr};
        EventLoopGroupPtr subLoopGroup_ {nullptr};
    };
} // namespace nets::net

#endif // NETS_NET_SERVER_BOOTSTRAP_H
