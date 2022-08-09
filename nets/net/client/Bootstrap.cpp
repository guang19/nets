//
// Created by guang19 on 2022/5/18.
//

#include "nets/net/client/Bootstrap.h"

namespace nets::net
{
    Bootstrap::Bootstrap() : channelHandlers_(), channelInitializationCallback_() {}

    Bootstrap& Bootstrap::handler(ChannelHandlerRawPtr channelHandler)
    {
        channelHandlers_.push_back(ChannelHandlerPtr(channelHandler));
        return *this;
    }

    Bootstrap& Bootstrap::handler(const ChannelHandlerPtr& channelHandler)
    {
        channelHandlers_.push_back(channelHandler);
        return *this;
    }

    Bootstrap& Bootstrap::handler(const ChannelInitializationCallback& channelInitializationCallback)
    {
        channelInitializationCallback_ = channelInitializationCallback;
        return *this;
    }

    Bootstrap& Bootstrap::connect(const char* ip, PortType port, bool ipv6)
    {
        connect(InetSockAddress(ip, port, ipv6));
        return *this;
    }

    Bootstrap& Bootstrap::connect(const InetSockAddress& serverAddress)
    {
        doConnect(serverAddress);
        return *this;
    }

    void Bootstrap::doConnect(const InetSockAddress& serverAddress)
    {
        FdType sockFd = socket::createTcpSocket(serverAddress.ipFamily());
        InetSockAddress localAddr {};
        socket::getLocalAddress(sockFd, localAddr.sockAddr());
        auto socketChannel = ::std::make_shared<SocketChannel>(sockFd, localAddr, serverAddress, mainLoopGroup_->next());
        ChannelOptionList channelOptions {::std::move(channelOptions_)};
        assert(channelOptions_.empty());
        socketChannel->setChannelOptions(channelOptions);

        ChannelHandlerList channelHandlers {::std::move(channelHandlers_)};
        assert(channelHandlers_.empty());
        for (const auto& channelHandler: channelHandlers)
        {
            assert(channelHandler.use_count() == 1);
            socketChannel->pipeline().addLast(channelHandler);
        }
        ChannelInitializationCallback channelInitializationCallback {::std::move(channelInitializationCallback_)};
        assert(channelInitializationCallback_ == nullptr);
        if (channelInitializationCallback != nullptr)
        {
            channelInitializationCallback(*socketChannel);
        }
        socketChannel->init();
    }
} // namespace nets::net