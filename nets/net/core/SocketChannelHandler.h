//
// Created by guang19
//

#ifndef NETS_SOCKET_CHANNEL_HANDLER_H
#define NETS_SOCKET_CHANNEL_HANDLER_H

#include "nets/net/core/InetSockAddress.h"

namespace nets
{
    class ByteBuffer;
    class SocketChannelContext;
    class SocketChannelHandler
    {
    public:
        SocketChannelHandler() = default;
        virtual ~SocketChannelHandler() = default;

    public:
        virtual void channelConnect(SocketChannelContext& channelContext, const InetSockAddress& localAddress,
                                    const InetSockAddress& peerAddress);
        virtual void channelDisconnect(SocketChannelContext& channelContext);
        virtual void channelRead(SocketChannelContext& channelContext, ByteBuffer& message);
    };
} // namespace nets

#endif // NETS_SOCKET_CHANNEL_HANDLER_H