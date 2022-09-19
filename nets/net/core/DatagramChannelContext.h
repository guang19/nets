//
// Created by guang19
//

#ifndef NETS_NET_DATAGRAM_CHANNEL_CONTEXT_H
#define NETS_NET_DATAGRAM_CHANNEL_CONTEXT_H

#include "nets/net/core/ChannelContext.h"
#include "nets/net/core/DatagramChannel.h"

namespace nets::net
{
    class DatagramChannelContext : public ChannelContext
    {
    public:
        using DatagramChannelRawPtr = DatagramChannel*;

    public:
        explicit DatagramChannelContext(DatagramChannelRawPtr channel);
        ~DatagramChannelContext() = default;

    private:
        DatagramChannelRawPtr channel_ {nullptr};
    };
} // namespace nets::net

#endif // NETS_NET_DATAGRAM_CHANNEL_CONTEXT_H
