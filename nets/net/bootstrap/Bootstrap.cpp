//
// Created by guang19
//

#include "nets/net/bootstrap/Bootstrap.h"

namespace nets::net
{
    Bootstrap::Bootstrap()
        : AbstractBootstrap(), retry_(false), retryInterval_(0), socketChannelHandlers_(), datagramChannelHandlers_(),
          socketChannelInitializationCallback_(), datagramChannelInitializationCallback_()
    {
    }

    Bootstrap& Bootstrap::channelHandler(SocketChannelHandlerRawPtr channelHandler)
    {
        socketChannelHandlers_.push_back(SocketChannelHandlerPtr(channelHandler));
        return *this;
    }

    Bootstrap& Bootstrap::channelHandler(const SocketChannelHandlerPtr& channelHandler)
    {
        socketChannelHandlers_.push_back(channelHandler);
        return *this;
    }

    Bootstrap& Bootstrap::channelHandler(const SocketChannelInitializationCallback& channelInitializationCallback)
    {
        socketChannelInitializationCallback_ = channelInitializationCallback;
        return *this;
    }

    Bootstrap& Bootstrap::channelHandler(DatagramChannelHandlerRawPtr channelHandler)
    {
        datagramChannelHandlers_.push_back(DatagramChannelHandlerPtr(channelHandler));
        return *this;
    }

    Bootstrap& Bootstrap::channelHandler(const DatagramChannelHandlerPtr& channelHandler)
    {
        datagramChannelHandlers_.push_back(channelHandler);
        return *this;
    }

    Bootstrap& Bootstrap::channelHandler(const DatagramChannelInitializationCallback& channelInitializationCallback)
    {
        datagramChannelInitializationCallback_ = channelInitializationCallback;
        return *this;
    }

    Bootstrap& Bootstrap::bind()
    {
        return bind(InetSockAddress());
    }

    Bootstrap& Bootstrap::bind(PortType port, bool ipv6)
    {
        return bind(InetSockAddress::createAnySockAddress(port, ipv6));
    }

    Bootstrap& Bootstrap::bind(const char* ip, PortType port, bool ipv6)
    {
        return bind(InetSockAddress(ip, port, ipv6));
    }

    Bootstrap& Bootstrap::bind(const InetSockAddress& localAddress)
    {
        mainLoopGroup_.loopEach();
        auto future = mainLoopGroup_.submit(
            [this, &localAddress]()
            {
                doBind(localAddress);
            });
        future.wait();
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

        if (!channelOptions_.empty())
        {
            connectorChannel->setChannelOptions(channelOptions_);
            channelOptions_.clear();
        }

        if (!socketChannelHandlers_.empty())
        {
            connectorChannel->setChannelHandlers(socketChannelHandlers_);
            socketChannelHandlers_.clear();
        }

        if (socketChannelInitializationCallback_ != nullptr)
        {
            connectorChannel->setChannelInitializationCallback(socketChannelInitializationCallback_);
            socketChannelInitializationCallback_ = nullptr;
        }

        assert(channelOptions_.empty());
        assert(socketChannelHandlers_.empty());
        assert(socketChannelInitializationCallback_ == nullptr);
    }

    void Bootstrap::doBind(const InetSockAddress& localAddress)
    {
        auto datagramChannel = ::std::make_shared<DatagramChannel>(mainLoopGroup_.next());
        initDatagramChannel(datagramChannel);
        datagramChannel->bind(localAddress);
    }

    void Bootstrap::initDatagramChannel(DatagramChannelPtr& datagramChannel)
    {
        if (!channelOptions_.empty())
        {
            datagramChannel->setChannelOptions(channelOptions_);
            channelOptions_.clear();
        }

        if (!datagramChannelHandlers_.empty())
        {
            for (const auto& childHandler: datagramChannelHandlers_)
            {
                assert(childHandler.use_count() == 1);
                datagramChannel->pipeline().addLast(childHandler);
            }
            datagramChannelHandlers_.clear();
        }

        if (datagramChannelInitializationCallback_ != nullptr)
        {
            datagramChannelInitializationCallback_(*datagramChannel);
            datagramChannelInitializationCallback_ = nullptr;
        }
        assert(channelOptions_.empty());
        assert(datagramChannelHandlers_.empty());
        assert(datagramChannelInitializationCallback_ == nullptr);
    }
} // namespace nets::net