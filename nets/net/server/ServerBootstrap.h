//
// Created by guang19
//

#ifndef NETS_NET_SERVER_BOOTSTRAP_H
#define NETS_NET_SERVER_BOOTSTRAP_H

#include "nets/net/core/AbstractBootstrap.h"
#include "nets/net/core/ChannelHandler.h"
#include "nets/net/server/ServerSocketChannel.h"

namespace nets::net
{
    class ServerBootstrap : public AbstractBootstrap<ServerBootstrap>
    {
    public:
        using ChannelHandlerRawPtr = typename ServerSocketChannel::ChannelHandlerRawPtr;
        using ChannelHandlerPtr = typename ServerSocketChannel::ChannelHandlerPtr;
        using ChannelHandlerList = typename ServerSocketChannel::ChannelHandlerList;
        using ChannelInitializationCallback = typename ServerSocketChannel::ChannelInitializationCallback;

    public:
        explicit ServerBootstrap(IntType numOfChildEventLoops);
        ~ServerBootstrap() = default;

    public:
        ServerBootstrap& childOption(const ChannelOption& channelOption, const ChannelOption::ValueType& value);
        // set the ChannelHandler shared by all channels
        ServerBootstrap& childHandler(ChannelHandlerRawPtr childHandler);
        ServerBootstrap& childHandler(const ChannelHandlerPtr& childHandler);
        // set the ChannelHandler for each channel
        ServerBootstrap& childHandler(const ChannelInitializationCallback& childInitializationCallback);

        ServerBootstrap& bind(PortType port, bool ipv6 = false);
        ServerBootstrap& bind(const char* ip, PortType port, bool ipv6 = false);
        ServerBootstrap& bind(const InetSockAddress& localAddress);

        void sync();

        void shutdown() override;
        bool isShutdown() const override;

    private:
        void doBind(const InetSockAddress& localAddress);

    private:
        ChannelOptionList childOptions_ {};
        ChannelHandlerList childHandlers_ {};
        ChannelInitializationCallback childInitializationCallback_ {};
        EventLoopGroup childLoopGroup_;

        static constexpr char ChildEventLoopGroupName[] = "ChildLoopGroup";
        static const IntType DefaultNumbOfChildEventLoops;
    };
} // namespace nets::net

#endif // NETS_NET_SERVER_BOOTSTRAP_H