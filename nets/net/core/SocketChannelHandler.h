//
// Created by guang19
//

#ifndef NETS_NET_SOCKET_CHANNEL_HANDLER_H
#define NETS_NET_SOCKET_CHANNEL_HANDLER_H

#include "nets/net/core/ChannelContext.h"
#include "nets/net/core/ChannelHandler.h"
#include "nets/net/core/InetSockAddress.h"

namespace nets::net
{
    class ByteBuffer;
    class SocketChannelHandler : public ChannelHandler
    {
    public:
        SocketChannelHandler() = default;
        virtual ~SocketChannelHandler() override = default;

    public:
        virtual void channelConnect(ChannelContext& channelContext, const InetSockAddress& localAddress,
                                    const InetSockAddress& peerAddress);
        virtual void channelDisconnect(ChannelContext& channelContext);
        virtual void channelRead(ChannelContext& channelContext, ByteBuffer& message);
    };
} // namespace nets::net

#endif // NETS_NET_SOCKET_CHANNEL_HANDLER_H