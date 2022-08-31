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

    void ServerBootstrap::doBind(const InetSockAddress& localAddress)
    {
        auto serverSocketChannel = ::std::make_shared<ServerSocketChannel>(mainLoopGroup_.next());
        serverSocketChannel->setNextEventLoopFn(
            [this]() -> EventLoopGroup::EventLoopRawPtr
            {
                return childLoopGroup_.next();
            });
        ChannelOptionList channelOptions {};
        channelOptions.swap(channelOptions_);
        assert(channelOptions_.empty());
        serverSocketChannel->setChannelOptions(channelOptions);

        ChannelOptionList childOptions {};
        childOptions.swap(childOptions_);
        assert(childOptions_.empty());
        serverSocketChannel->setChildOptions(childOptions);

        ChannelHandlerList childHandlers {};
        childHandlers.swap(childHandlers_);
        assert(childHandlers_.empty());
        serverSocketChannel->setChildHandlers(childHandlers);

        ChannelInitializationCallback childInitializationCallback {};
        childInitializationCallback.swap(childInitializationCallback_);
        assert(childInitializationCallback_ == nullptr);
        serverSocketChannel->setChildInitializationCallback(childInitializationCallback);
        serverSocketChannel->bind(localAddress);
    }

    void ServerBootstrap::handleSignal(SignalHandler::SignoType signo)
    {
        switch (signo)
        {
            case SIGPIPE:
                break;
            case SIGHUP:
            case SIGINT:
            case SIGQUIT:
            case SIGTERM:
                // terminate server
                break;
        }
    }
} // namespace nets::net