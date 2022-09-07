//
// Created by guang19
//

#include "nets/net/client/Bootstrap.h"

namespace nets::net
{
    Bootstrap::Bootstrap()
        : AbstractBootstrap(), retry_(false), retryInterval_(0), channelHandlers_(),
          channelInitializationCallback_()
    {
    }

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

    Bootstrap& Bootstrap::bind()
    {
        return *this;
    }

    Bootstrap& Bootstrap::bind(const char* ip, PortType port, bool ipv6)
    {
        bind(InetSockAddress(ip, port, ipv6));
        return *this;
    }

    Bootstrap& Bootstrap::bind(const InetSockAddress& serverAddress)
    {

    }

    Bootstrap& Bootstrap::retry(bool retry, TimeType interval)
    {
        retry_ = retry;
        retryInterval_ = interval;
        return *this;
    }

    Bootstrap& Bootstrap::connect(const char* ip, PortType port, bool ipv6)
    {
        connect(InetSockAddress(ip, port, ipv6));
        return *this;
    }

    Bootstrap& Bootstrap::connect(const InetSockAddress& serverAddress)
    {
        mainLoopGroup_.loopEach();
        auto future = mainLoopGroup_.submit(
            [this, &serverAddress]()
            {
                doConnect(serverAddress);
            });
        future.wait();
        return *this;
    }

    void Bootstrap::sync()
    {
        mainLoopGroup_.syncEach();
    }

    void Bootstrap::doConnect(const InetSockAddress& serverAddress)
    {
        auto connectorChannel = ::std::make_shared<ConnectorChannel>(mainLoopGroup_.next());
        initConnectorChannel(connectorChannel);
        connectorChannel->connect(serverAddress);
    }

    void Bootstrap::initConnectorChannel(ConnectorChannelPtr& connectorChannel)
    {
        connectorChannel->setRetry(retry_);
        connectorChannel->setRetryInterval(retryInterval_);

        ChannelOptionList channelOptions(channelOptions_);
        connectorChannel->setChannelOptions(channelOptions);
        channelOptions_.clear();

        ChannelHandlerList channelHandlers(channelHandlers_);
        connectorChannel->setChannelHandlers(channelHandlers);
        channelHandlers_.clear();

        ChannelInitializationCallback channelInitializationCallback(channelInitializationCallback_);
        connectorChannel->setChannelInitializationCallback(channelInitializationCallback);
        channelInitializationCallback_ = nullptr;

        assert(channelOptions_.empty());
        assert(channelHandlers_.empty());
        assert(channelInitializationCallback_ == nullptr);
    }

    void Bootstrap::doBind(const InetSockAddress& serverAddress)
    {

    }
} // namespace nets::net