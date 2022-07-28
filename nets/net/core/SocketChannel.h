//
// Created by guang19 on 2022/5/19.
//

#ifndef NETS_NET_SOCKET_CHANNEL_H
#define NETS_NET_SOCKET_CHANNEL_H

#include "nets/net/core/Channel.h"
#include "nets/net/core/ChannelHandlerPipeline.h"

namespace nets::net
{
    class SocketChannel : public Channel
    {
    public:
        using ChannelHandlerPipelineRawPtr = ChannelHandlerPipeline*;

    public:
        explicit SocketChannel(FdType sockFd, const InetSockAddress& peerAddress, EventLoopRawPtr eventLoop);
        ~SocketChannel() override = default;

    public:
        inline FdType fd() const override
        {
            return sockFd_;
        }

        inline ChannelHandlerPipelineRawPtr pipeline()
        {
            return &channelHandlerPipeline_;
        }

        inline const InetSockAddress& localAddress() const
        {
            return localAddress_;
        }

        inline const InetSockAddress& peerAddress() const
        {
            return peerAddress_;
        }

        void setChannelOptions(const ChannelOptionList& channelOptions);

    public:
        void handleReadEvent() override;
        void handleWriteEvent() override;
        void handleErrorEvent() override;

    private:
        FdType sockFd_ {socket::InvalidFd};
        InetSockAddress localAddress_ {};
        InetSockAddress peerAddress_ {};
        ChannelHandlerPipeline channelHandlerPipeline_ {nullptr};
    };
} // namespace nets::net

#endif // NETS_NET_SOCKET_CHANNEL_H
