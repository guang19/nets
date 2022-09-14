//
// Created by guang19
//

#ifndef NETS_NET_SOCKET_CHANNEL_HANDLER_H
#define NETS_NET_SOCKET_CHANNEL_HANDLER_H

#include "nets/net/core/ChannelHandler.h"
#include "nets/net/core/SocketChannelContext.h"

namespace nets::net
{
    class SocketChannelHandler : public ChannelHandler
    {
    public:
        SocketChannelHandler() = default;
        virtual ~SocketChannelHandler() override = default;

    public:
        virtual void channelConnect(SocketChannelContext& channelContext, const InetSockAddress& localAddress,
                                    const InetSockAddress& peerAddress);
        virtual void channelDisconnect(SocketChannelContext& channelContext);
        virtual void channelRead(SocketChannelContext& channelContext, ByteBuffer& message);
    };
} // namespace nets::net

#endif // NETS_NET_SOCKET_CHANNEL_HANDLER_H