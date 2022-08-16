//
// Created by guang19
//

#ifndef NETS_NET_CHANNEL_HANDLER_H
#define NETS_NET_CHANNEL_HANDLER_H

#include "nets/base/Noncopyable.h"
#include "nets/net/core/ChannelContext.h"
#include "nets/net/core/InetSockAddress.h"

namespace nets::net
{
    class ByteBuffer;
    class ChannelHandler : nets::base::Noncopyable
    {
    public:
        ChannelHandler() = default;
        virtual ~ChannelHandler() = default;

    public:
        virtual void channelConnect(ChannelContext& channelContext, const InetSockAddress& localAddress,
                                    const InetSockAddress& peerAddress);
        virtual void channelDisconnect(ChannelContext& channelContext);
        virtual void channelRead(ChannelContext& channelContext, ByteBuffer& message);
        virtual void channelWriteComplete(ChannelContext& channelContext);
        virtual void exceptionCaught(ChannelContext& channelContext, const ::std::exception& exception);
    };
} // namespace nets::net

#endif // NETS_NET_CHANNEL_HANDLER_H