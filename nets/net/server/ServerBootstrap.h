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
        explicit ServerBootstrap(NType numOfMainEventLoops, NType numOfChildEventLoops);
        ~ServerBootstrap() = default;

    public:
        ServerBootstrap& childOption(const ChannelOption& channelOption, ChannelOption::ValueType value);
        // set the ChannelHandler shared by all channels
        ServerBootstrap& childHandler(const ChannelHandlerPtr& childHandler);
        // set the ChannelHandler for each channel
        ServerBootstrap& childHandler(const ChannelInitializationCallback& childInitializationCallback);

        ServerBootstrap& bind(PortType port, bool ipv6 = false);
        ServerBootstrap& bind(const char* ip, PortType port, bool ipv6 = false);
        ServerBootstrap& bind(const InetSockAddress& localAddress);

        void launch();

    private:
        void doBind(const InetSockAddress& localAddress);

    private:
        ::std::atomic_bool running_ {false};
        ChannelOptionSet childOptions_ {};
        ChannelHandlerList childHandlers_ {};
        ChannelInitializationCallback childInitializationCallback_ {};
        EventLoopGroupPtr childLoopGroup_ {nullptr};
    };
} // namespace nets::net

#endif // NETS_NET_SERVER_BOOTSTRAP_H
