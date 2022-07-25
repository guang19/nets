//
// Created by guang19 on 2022/5/18.
//

#ifndef NETS_NET_SERVER_BOOTSTRAP_H
#define NETS_NET_SERVER_BOOTSTRAP_H

#include "nets/net/core/AbstractBootstrap.h"
#include "nets/net/core/ChannelHandler.h"
#include "nets/net/server/ServerSocketChannel.h"

namespace nets::net
{
    class ServerBootstrap : public AbstractBootstrap
    {
    public:
        using ChannelHandlerPtr = typename ServerSocketChannel::ChannelHandlerPtr;
        using ChannelHandlerList = typename ServerSocketChannel::ChannelHandlerList;
        using ChannelInitializationCallback = typename ServerSocketChannel::ChannelInitializationCallback;

    public:
        explicit ServerBootstrap(NType numOfMainEventLoops, NType numOfSubEventLoops);
        ~ServerBootstrap() = default;

    public:
        ServerBootstrap& childOption(const ChannelOption& channelOption, ChannelOption::ValueType value);
        // set the ChannelHandler shared by all channels
        ServerBootstrap& childHandler(const ChannelHandlerPtr& channelHandler);
        // set the ChannelHandler for each channel
        ServerBootstrap& childHandler(const ChannelInitializationCallback& channelInitializationCallback);

        ServerBootstrap& bind(PortType port, bool ipv6 = false);
        ServerBootstrap& bind(const char* ip, PortType port, bool ipv6 = false);
        ServerBootstrap& bind(const InetSockAddress& localAddress);

        void launch();

    private:
        void doBind(const InetSockAddress& localAddress);

    private:
        ::std::atomic_bool running_ {false};
        ChannelHandlerList channelHandlers_ {};
        ChannelInitializationCallback channelInitializationCallback_ {};
        EventLoopGroupPtr subLoopGroup_ {nullptr};
    };
} // namespace nets::net

#endif // NETS_NET_SERVER_BOOTSTRAP_H
