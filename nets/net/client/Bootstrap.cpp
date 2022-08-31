//
// Created by guang19
//

#include "nets/net/client/Bootstrap.h"

namespace nets::net
{
    Bootstrap::Bootstrap() : retry_(false), retryInterval_(0), channelHandlers_(), channelInitializationCallback_() {}

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
        mainLoopGroup_->loopEach();
        auto future = mainLoopGroup_->submit(
            [this, serverAddress]()
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
        auto connectorChannel = ::std::make_shared<ConnectorChannel>(mainLoopGroup_->next());
        initConnectorChannel(connectorChannel);
        connectorChannel->connect(serverAddress);
    }

    void Bootstrap::initConnectorChannel(ConnectorChannelPtr& connectorChannel)
    {
        connectorChannel->setRetry(retry_);
        connectorChannel->setRetryInterval(retryInterval_);

        ChannelOptionList channelOptions {};
        channelOptions.swap(channelOptions_);
        assert(channelOptions_.empty());
        connectorChannel->setChannelOptions(channelOptions);

        ChannelHandlerList channelHandlers {};
        channelHandlers.swap(channelHandlers_);
        assert(channelHandlers_.empty());
        connectorChannel->setChannelHandlers(channelHandlers);

        ChannelInitializationCallback channelInitializationCallback {};
        channelInitializationCallback.swap(channelInitializationCallback_);
        assert(channelInitializationCallback_ == nullptr);
        connectorChannel->setChannelInitializationCallback(channelInitializationCallback);
    }

    void Bootstrap::handleSignal(base::SignalHandler::SignoType signo)
    {
        switch (signo)
        {
            case SIGPIPE:
                break;
            case SIGHUP:
            case SIGINT:
            case SIGQUIT:
            case SIGTERM:
                // terminate connection
                break;
        }
    }
} // namespace nets::net