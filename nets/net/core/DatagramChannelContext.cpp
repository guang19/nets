//
// Created by guang19
//

#include "nets/net/core/DatagramChannelContext.h"

#include "nets/net/channel/DatagramChannel.h"

namespace nets::net
{
    DatagramChannelContext::DatagramChannelContext(DatagramChannelRawPtr channel) : channel_(channel) {}

    SSizeType DatagramChannelContext::write(const void* message, SizeType length, const InetSockAddress& recipient)
    {
        return channel_->write(message, length, recipient);
    }

    SSizeType DatagramChannelContext::write(const StringType& message, const InetSockAddress& recipient)
    {
        return channel_->write(message, recipient);
    }

    SSizeType DatagramChannelContext::write(const ByteBuffer& message, const InetSockAddress& recipient)
    {
        return channel_->write(message, recipient);
    }

    SSizeType DatagramChannelContext::write(const DatagramPacket& message)
    {
        return channel_->write(message);
    }
} // namespace nets::net