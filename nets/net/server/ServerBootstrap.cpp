//
// Created by guang19 on 2022/5/18.
//

#include "nets/net/server/ServerBootstrap.h"

#include <unistd.h>

namespace nets::net
{
    namespace
    {
        constexpr char ChildEventLoopGroupName[] = "ChildLoopGroup";
        const ServerBootstrap::IntType DefaultNumbOfChildEventLoops = ::sysconf(_SC_NPROCESSORS_ONLN) << 1;
    } // namespace

    ServerBootstrap::ServerBootstrap(IntType numOfChildEventLoops)
        : AbstractBootstrap(), running_(false), childOptions_(), childHandlers_(), childInitializationCallback_()
    {
        numOfChildEventLoops = numOfChildEventLoops == 0 ? DefaultNumbOfChildEventLoops : numOfChildEventLoops;
        childLoopGroup_ = ::std::make_unique<EventLoopGroup>(numOfChildEventLoops, ChildEventLoopGroupName);
    }

    ServerBootstrap& ServerBootstrap::childOption(const ChannelOption& channelOption, const ChannelOption::ValueType& value)
    {
        childOptions_.emplace_back(channelOption.sockOpt(), value);
        return *this;
    }

    ServerBootstrap& ServerBootstrap::childHandler(ChannelHandlerRawPtr childHandler)
    {
        childHandlers_.push_back(ChannelHandlerPtr(childHandler));
        return *this;
    }

    ServerBootstrap& ServerBootstrap::childHandler(const ChannelHandlerPtr& childHandler)
    {
        childHandlers_.push_back(childHandler);
        return *this;
    }

    ServerBootstrap& ServerBootstrap::childHandler(const ChannelInitializationCallback& childInitializationCallback)
    {
        childInitializationCallback_ = childInitializationCallback;
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
        auto future = mainLoopGroup_->submit(
            [this, localAddress]
            {
                doBind(localAddress);
            });
        future.wait();
        return *this;
    }

    void ServerBootstrap::sync()
    {
        if (running_)
        {
            LOGS_DEBUG << "ServerBootstrap has started";
            return;
        }
        running_ = true;
        childLoopGroup_->loopEach();
        mainLoopGroup_->syncEach();
        childLoopGroup_->syncEach();
    }

    void ServerBootstrap::doBind(const InetSockAddress& localAddress)
    {
        auto serverSocketChannel = ::std::make_shared<ServerSocketChannel>(mainLoopGroup_->next());
        serverSocketChannel->setNextEventLoopFn(
            [this]() -> EventLoopGroup::EventLoopRawPtr
            {
                return childLoopGroup_->next();
            });
        ChannelOptionList channelOptions {::std::move(channelOptions_)};
        assert(channelOptions_.empty());
        serverSocketChannel->setChannelOptions(channelOptions);
        ChannelOptionList childOptions {::std::move(childOptions_)};
        assert(childOptions_.empty());
        serverSocketChannel->setChildOptions(childOptions);

        ChannelHandlerList childHandlers {::std::move(childHandlers_)};
        assert(childHandlers_.empty());
        serverSocketChannel->setChildHandlers(childHandlers);
        ChannelInitializationCallback childInitializationCallback {::std::move(childInitializationCallback_)};
        assert(childInitializationCallback_ == nullptr);
        serverSocketChannel->setChildInitializationCallback(childInitializationCallback);
        serverSocketChannel->bind(localAddress);
    }
} // namespace nets::net