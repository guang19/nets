//
// Created by guang19 on 2022/5/18.
//

#include "nets/net/server/ServerBootstrap.h"

#include <unistd.h>

namespace nets::net
{
    namespace
    {
        constexpr char SubEventLoopGroupName[] = "SubLoopGroup";
        const AbstractBootstrap::NType DefaultNumbOfSubEventLoops = ::sysconf(_SC_NPROCESSORS_ONLN) << 1;
    } // namespace

    ServerBootstrap::ServerBootstrap(NType numOfMainEventLoops, NType numOfSubEventLoops)
        : AbstractBootstrap(numOfMainEventLoops), running_(false)
    {
        numOfSubEventLoops = numOfSubEventLoops <= 0 ? DefaultNumbOfSubEventLoops : numOfSubEventLoops;
        subLoopGroup_ = ::std::make_unique<EventLoopGroup>(numOfSubEventLoops, SubEventLoopGroupName);
    }

    ServerBootstrap& ServerBootstrap::childHandler(const ChannelHandlerPtr& channelHandler)
    {
        channelHandlers_.push_back(channelHandler);
        return *this;
    }

    ServerBootstrap& ServerBootstrap::childHandler(const ChannelInitializationCallback& channelInitializationCallback)
    {
        channelInitializationCallback_ = channelInitializationCallback;
        return *this;
    }

    ServerBootstrap& ServerBootstrap::bind(const char* ip, PortType port, bool ipv6)
    {
        bind(InetSockAddress(ip, port, ipv6));
        return *this;
    }

    ServerBootstrap& ServerBootstrap::bind(PortType port, bool ipv6)
    {
        bind(InetSockAddress::createAnySockAddress(port, ipv6));
        return *this;
    }

    ServerBootstrap& ServerBootstrap::bind(const InetSockAddress& localAddress)
    {
        mainLoopGroup_->loopEach();
        // Clang-Tidy: Prefer a lambda to std::bind
        auto future = mainLoopGroup_->submit(
            [this, localAddress]
            {
                doBind(localAddress);
            });
        future.wait();
        return *this;
    }

    void ServerBootstrap::doBind(const InetSockAddress& localAddress)
    {
        auto serverSocketChannel = ::std::make_shared<ServerSocketChannel>(mainLoopGroup_->next());
        serverSocketChannel->setNextEventLoopFn(
            [this]() -> EventLoopGroup::EventLoopRawPtr
            {
                return subLoopGroup_->next();
            });
        ChannelHandlerList channelHandlers {::std::move(channelHandlers_)};
        assert(channelHandlers_.empty());
        serverSocketChannel->setChannelHandlers(channelHandlers_);
        ChannelInitializationCallback channelInitializationCallback {::std::move(channelInitializationCallback_)};
        assert(channelInitializationCallback_ == nullptr);
        serverSocketChannel->setChannelInitializationCallback(channelInitializationCallback);
        serverSocketChannel->bind(localAddress);
    }

    void ServerBootstrap::launch()
    {
        if (running_)
        {
            LOGS_DEBUG << "ServerBootstrap has started";
            return;
        }
        running_ = true;
        subLoopGroup_->loopEach();
        mainLoopGroup_->syncEach();
    }
} // namespace nets::net