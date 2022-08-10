//
// Created by guang19 on 2022/5/18.
//

#include "nets/net/client/Bootstrap.h"

namespace nets::net
{
    Bootstrap::Bootstrap() : channelHandlers_(), channelInitializationCallback_() {}

    Bootstrap& Bootstrap::channelHandler(ChannelHandlerRawPtr channelHandler)
    {
        channelHandlers_.push_back(ChannelHandlerPtr(channelHandler));
        return *this;
    }

    Bootstrap& Bootstrap::channelHandler(const ChannelHandlerPtr& channelHandler)
    {
        channelHandlers_.push_back(channelHandler);
        return *this;
    }

    Bootstrap& Bootstrap::channelHandler(const ChannelInitializationCallback& channelInitializationCallback)
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
        mainLoopGroup_->loopEach();
        auto future = mainLoopGroup_->submit(
            [this, serverAddress]
            {
                doConnect(serverAddress);
            });
        future.wait();
        return *this;
    }

    void Bootstrap::sync()
    {
        mainLoopGroup_->syncEach();
    }

    void Bootstrap::doConnect(const InetSockAddress& serverAddress)
    {
        FdType sockFd = socket::createTcpSocket(serverAddress.ipFamily());
        socket::setSockNonBlock(sockFd, true);
        InetSockAddress localAddr {};
        socket::getLocalAddress(sockFd, localAddr.sockAddr());
        auto socketChannel = ::std::make_shared<SocketChannel>(sockFd, localAddr, serverAddress, mainLoopGroup_->next());
        initSocketChannel(socketChannel);
        socketChannel->connect();
    }

    void Bootstrap::initSocketChannel(::std::shared_ptr<SocketChannel>& socketChannel)
    {
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
    }
} // namespace nets::net