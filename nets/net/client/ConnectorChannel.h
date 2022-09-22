//
// Created by guang19
//

#ifndef NETS_NET_CONNECTOR_CHANNEL_H
#define NETS_NET_CONNECTOR_CHANNEL_H

#include <functional>

#include "nets/net/core/SocketChannel.h"

namespace nets::net
{
    class ConnectorChannel : public Channel
    {
    public:
        using TimeType = ::time_t;
        using SocketChannelPtr = ::std::shared_ptr<SocketChannel>;
        using ChannelHandlerList = typename SocketChannelHandlerPipeline::SocketChannelHandlerList;
        using ChannelInitializationCallback = ::std::function<void(SocketChannel& channel)>;

    public:
        explicit ConnectorChannel(EventLoopRawPtr eventLoop);
        ~ConnectorChannel() override;

    public:
        FdType fd() const override;

    protected:
        void handleWriteEvent() override;
        void handleErrorEvent() override;

    public:
        inline void setRetry(bool retry)
        {
            retry_ = retry;
        }

        inline void setRetryInterval(TimeType retryInterval)
        {
            retryInterval_ = retryInterval;
        }

        inline void setChannelOptions(const ChannelOptionList& channelOptions)
        {
            channelOptions_ = channelOptions;
        }

        inline void setChannelHandlers(const ChannelHandlerList& channelHandlers)
        {
            channelHandlers_ = channelHandlers;
        }

        inline void setChannelInitializationCallback(const ChannelInitializationCallback& channelInitializationCallback)
        {
            channelInitializationCallback_ = channelInitializationCallback;
        }

        void connect(const InetSockAddress& serverAddress);

    private:
        void initSocketChannel(SocketChannelPtr& socketChannel);
        void newSocketChannel();
        void handleConnectError(::int32_t errNum);
        void waitConnect();
        void reconnect();

    private:
        FdType sockFd_ {socket::InvalidFd};
        InetSockAddress localAddress_ {};
        InetSockAddress peerAddress_ {};

        enum ConnectionState
        {
            CONNECTED,
            CONNECTING,
            DISCONNECTED
        };
        ConnectionState state_;
        bool retry_ {false};
        TimeType retryInterval_ {0};
        ChannelOptionList channelOptions_ {};
        ChannelHandlerList channelHandlers_ {};
        ChannelInitializationCallback channelInitializationCallback_ {};
    };
} // namespace nets::net

#endif // NETS_NET_CONNECTOR_CHANNEL_H