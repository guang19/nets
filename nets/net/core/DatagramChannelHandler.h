//
// Created by guang19
//

#ifndef NETS_NET_DATAGRAM_CHANNEL_HANDLER_H
#define NETS_NET_DATAGRAM_CHANNEL_HANDLER_H

#include "nets/net/core/ChannelHandler.h"
#include "nets/net/core/DatagramChannelContext.h"

namespace nets::net
{
    class DatagramChannelHandler : public ChannelHandler
    {
    public:
        DatagramChannelHandler() = default;
        virtual ~DatagramChannelHandler() override = default;

    public:
        virtual void channelActive(DatagramChannelContext& channelContext);
        virtual void channelRead(DatagramChannelContext& channelContext, DatagramPacket& message);
    };
} // namespace nets::net

#endif // NETS_NET_DATAGRAM_CHANNEL_HANDLER_H