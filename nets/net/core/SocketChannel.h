//
// Created by guang19 on 2022/5/19.
//

#ifndef NETS_NET_SOCKET_CHANNEL_H
#define NETS_NET_SOCKET_CHANNEL_H

#include "nets/net/core/Channel.h"

namespace nets::net
{
    class SocketChannel : public Channel
    {
    public:
        explicit SocketChannel(FdType sockFd, const InetSockAddress& peerAddress, EventLoopRawPtr eventLoop);
        ~SocketChannel() override = default;

    public:
        inline FdType fd() const override
        {
            return sockFd_;
        }

        inline ChannelContextRawPtr channelContext() override
        {
            return &channelContext_;
        }

        inline ChannelHandlerPipelineRawPtr channelHandlerPipeline() override
        {
            return channelContext_.pipeline();
        }

        void setChannelOptions(const ChannelOptionList& channelOptions) override;

    public:
        void handleReadEvent() override;
        void handleWriteEvent() override;
        void handleErrorEvent() override;

    private:
        FdType sockFd_ {socket::InvalidFd};
        InetSockAddress peerAddress_ {};
        ChannelContext channelContext_ {nullptr};
    };
} // namespace nets::net

#endif // NETS_NET_SOCKET_CHANNEL_H
