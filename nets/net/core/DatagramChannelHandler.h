//
// Created by guang19
//

#ifndef NETS_DATAGRAM_CHANNEL_HANDLER_H
#define NETS_DATAGRAM_CHANNEL_HANDLER_H

#include "nets/net/core/DatagramPacket.h"

namespace nets
{
    class DatagramChannelContext;
    class DatagramChannelHandler
    {
    public:
        DatagramChannelHandler() = default;
        virtual ~DatagramChannelHandler() = default;

    public:
        virtual void channelActive(DatagramChannelContext& channelContext);
        virtual void channelRead(DatagramChannelContext& channelContext, DatagramPacket& message);
    };
} // namespace nets

#endif // NETS_DATAGRAM_CHANNEL_HANDLER_H