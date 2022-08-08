//
// Created by guang19 on 2022/5/23.
//

#ifndef NETS_NET_CHANNEL_HANDLER_H
#define NETS_NET_CHANNEL_HANDLER_H

#include "nets/base/Noncopyable.h"
#include "nets/net/core/InetSockAddress.h"

namespace nets::net
{
    class ByteBuffer;
    class ChannelContext;

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
        virtual void exceptionCaught(ChannelContext& channelContext, const ::std::exception& exception);
    };
} // namespace nets::net

#endif // NETS_NET_CHANNEL_HANDLER_H
