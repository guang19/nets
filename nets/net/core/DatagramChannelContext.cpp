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

    bool DatagramChannelContext::joinIpv4MulticastGroupByIfAddr(const StringType& multicastAddr, const StringType& ifAddr)
    {
        return channel_->joinIpv4MulticastGroupByIfAddr(multicastAddr, ifAddr);
    }

    bool DatagramChannelContext::joinIpv4MulticastGroupByIfIndex(const StringType& multicastAddr, const StringType& inf)
    {
        return channel_->joinIpv4MulticastGroupByIfIndex(multicastAddr, inf);
    }

    bool DatagramChannelContext::joinIpv6MulticastGroup(const StringType& multicastAddr, const StringType& inf)
    {
        return channel_->joinIpv6MulticastGroup(multicastAddr, inf);
    }

    bool DatagramChannelContext::leaveIpv4MulticastGroupByIfAddr(const StringType& multicastAddr, const StringType& ifAddr)
    {
        return channel_->leaveIpv4MulticastGroupByIfAddr(multicastAddr, ifAddr);
    }

    bool DatagramChannelContext::leaveIpv4MulticastGroupByIfIndex(const StringType& multicastAddr, const StringType& inf)
    {
        return channel_->leaveIpv4MulticastGroupByIfIndex(multicastAddr, inf);
    }

    bool DatagramChannelContext::leaveIpv6MulticastGroup(const StringType& multicastAddr, const StringType& inf)
    {
        return channel_->leaveIpv6MulticastGroup(multicastAddr, inf);
    }
} // namespace nets::net