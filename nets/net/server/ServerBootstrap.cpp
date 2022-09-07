//
// Created by guang19
//

#include "nets/net/server/ServerBootstrap.h"

namespace nets::net
{
    const ServerBootstrap::IntType ServerBootstrap::DefaultNumbOfChildEventLoops = ::sysconf(_SC_NPROCESSORS_ONLN) << 1;

    ServerBootstrap::ServerBootstrap(IntType numOfChildEventLoops)
        : AbstractBootstrap(), childOptions_(), childHandlers_(), childInitializationCallback_(),
          childLoopGroup_(numOfChildEventLoops == 0 ? DefaultNumbOfChildEventLoops : numOfChildEventLoops,
                          ChildEventLoopGroupName)
    {
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
        return bind(InetSockAddress(ip, port, ipv6));
    }

    ServerBootstrap& ServerBootstrap::bind(PortType port, bool ipv6)
    {
        return bind(InetSockAddress::createAnySockAddress(port, ipv6));
    }

    ServerBootstrap& ServerBootstrap::bind(const InetSockAddress& localAddress)
    {
        mainLoopGroup_.loopEach();
        auto future = mainLoopGroup_.submit(
            [this, &localAddress]
            {
                doBind(localAddress);
            });
        future.wait();
        return *this;
    }

    void ServerBootstrap::sync()
    {
        childLoopGroup_.loopEach();
        mainLoopGroup_.syncEach();
        childLoopGroup_.syncEach();
    }

    void ServerBootstrap::shutdown()
    {
        if (!mainLoopGroup_.isShutdown())
        {
            mainLoopGroup_.shutdown();
        }
        if (!childLoopGroup_.isShutdown())
        {
            childLoopGroup_.shutdown();
        }
    }

    bool ServerBootstrap::isShutdown() const
    {
        return mainLoopGroup_.isShutdown() && childLoopGroup_.isShutdown();
    }

    void ServerBootstrap::doBind(const InetSockAddress& localAddress)
    {
        auto serverSocketChannel = ::std::make_shared<ServerSocketChannel>(mainLoopGroup_.next());
        initServerSocketChannel(serverSocketChannel);
        serverSocketChannel->bind(localAddress);
    }

    void ServerBootstrap::initServerSocketChannel(ServerSocketChannelPtr& serverSocketChannel)
    {
        serverSocketChannel->setNextEventLoopFn(
            [this]() -> EventLoopGroup::EventLoopRawPtr
            {
                return childLoopGroup_.next();
            });
        ChannelOptionList channelOptions(channelOptions_);
        serverSocketChannel->setChannelOptions(channelOptions);
        channelOptions_.clear();

        ChannelOptionList childOptions(childOptions_);
        serverSocketChannel->setChildOptions(childOptions);
        childOptions_.clear();

        ChannelHandlerList childHandlers(childHandlers_);
        serverSocketChannel->setChildHandlers(childHandlers);
        childHandlers_.clear();

        ChannelInitializationCallback childInitializationCallback(childInitializationCallback_);
        serverSocketChannel->setChildInitializationCallback(childInitializationCallback);
        childInitializationCallback_ = nullptr;

        assert(channelOptions_.empty());
        assert(childOptions_.empty());
        assert(childHandlers_.empty());
        assert(childInitializationCallback_ == nullptr);
    }
} // namespace nets::net