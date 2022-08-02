//
// Created by guang19 on 2022/5/23.
//

#ifndef NETS_NET_CHANNEL_HANDLER_H
#define NETS_NET_CHANNEL_HANDLER_H

#include "nets/base/Noncopyable.h"
#include "nets/net/core/InetSockAddress.h"
#include "nets/net/core/NetPackage.h"

namespace nets::net
{
    class ChannelContext;

    class ChannelHandler : nets::base::Noncopyable
    {
    public:
        ChannelHandler() = default;
        virtual ~ChannelHandler() = default;

    public:
        virtual void channelConnect(ChannelContext& channelContext, const InetSockAddress& localAddress,
                                    const InetSockAddress& peerAddress);
        virtual void channelClose(ChannelContext& channelContext);
        virtual void channelDisconnect(ChannelContext& channelContext);

        virtual void channelRead(ChannelContext& channelContext, NetPackage& message);
        virtual void channelWrite(ChannelContext& channelContext, NetPackage& message);
    };
} // namespace nets::net

#endif // NETS_NET_CHANNEL_HANDLER_H
