// MIT License
//
// Copyright (c) 2022 guang19
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

// @brief Bootstrap for tcp server

#ifndef NETS_SERVER_BOOTSTRAP_H
#define NETS_SERVER_BOOTSTRAP_H

#include "nets/net/bootstrap/AbstractBootstrap.h"
#include "nets/net/channel/ServerSocketChannel.h"

namespace nets
{
    class ServerBootstrap : public AbstractBootstrap<ServerBootstrap>
    {
    public:
        using ServerSocketChannelPtr = ::std::shared_ptr<ServerSocketChannel>;
        using ChannelHandlerPtr = typename SocketChannelHandlerPipeline::SocketChannelHandlerPtr;
        using ChannelHandlerList = typename ServerSocketChannel::ChannelHandlerList;
        using ChannelInitializationCallback = typename ServerSocketChannel::ChannelInitializationCallback;

    public:
        explicit ServerBootstrap(IntType numOfChildEventLoops);
        ~ServerBootstrap() override = default;

    public:
        ServerBootstrap& childOption(SockOption sockOption, const ChannelOption::ValueType& value);
        // set the ChannelHandler shared by all channels
        ServerBootstrap& childHandler(const ChannelHandlerPtr& childHandler);
        // set the ChannelHandler for each channel
        ServerBootstrap& childHandler(const ChannelInitializationCallback& childInitializationCallback);

        // tcp server
        ServerBootstrap& bind(PortType port, bool ipv6 = false);
        ServerBootstrap& bind(const StringType& ip, PortType port, bool ipv6 = false);
        ServerBootstrap& bind(const InetSockAddress& localAddress);

        void sync();

        void shutdown() override;
        bool isShutdown() const override;

    private:
        void doBind(const InetSockAddress& localAddress);
        void initServerSocketChannel(ServerSocketChannelPtr& serverSocketChannel);

    private:
        ChannelOptionList childOptions_;
        ChannelHandlerList childHandlers_;
        ChannelInitializationCallback childInitializationCallback_;
        EventLoopGroup childLoopGroup_;

        static constexpr char kChildEventLoopGroupName[] = "ChildLoopGroup";
        static const IntType kDefaultNumbOfChildEventLoops;
    };
} // namespace nets

#endif // NETS_SERVER_BOOTSTRAP_H