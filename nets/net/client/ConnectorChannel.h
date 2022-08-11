//
// Created by guang19 on 2022/8/10.
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
        using SocketChannelPtr = ::std::shared_ptr<SocketChannel>;
        using ChannelHandlerList = typename ChannelHandlerPipeline::ChannelHandlerList;
        using ChannelInitializationCallback = ::std::function<void(SocketChannel& channel)>;

    public:
        explicit ConnectorChannel(EventLoopRawPtr eventLoop);
        ~ConnectorChannel() override;

    public:
        FdType fd() const override;
        void handleWriteEvent() override;
        void handleErrorEvent() override;

    public:
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
        void channelActive();
        void handleConnectError(int32_t errNum);
        void checkConnected();
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
        ChannelOptionList channelOptions_ {};
        ChannelHandlerList channelHandlers_ {};
        ChannelInitializationCallback channelInitializationCallback_ {};
    };
} // namespace nets::net

#endif // NETS_NET_CONNECTOR_CHANNEL_H
