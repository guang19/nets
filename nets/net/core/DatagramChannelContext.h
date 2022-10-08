//
// Created by guang19
//

#ifndef NETS_DATAGRAM_CHANNEL_CONTEXT_H
#define NETS_DATAGRAM_CHANNEL_CONTEXT_H

#include "nets/base/Noncopyable.h"
#include "nets/net/core/DatagramPacket.h"

namespace nets
{
    class DatagramChannel;
    class DatagramChannelContext : Noncopyable
    {
    public:
        using StringType = ::std::string;
        using DatagramChannelRawPtr = DatagramChannel*;

    public:
        explicit DatagramChannelContext(DatagramChannelRawPtr channel);
        ~DatagramChannelContext() = default;

    public:
        inline DatagramChannel& channel()
        {
            return *channel_;
        }

        SSizeType write(const void* message, SizeType length, const InetSockAddress& recipient);
        SSizeType write(const StringType& message, const InetSockAddress& recipient);
        SSizeType write(const ByteBuffer& message, const InetSockAddress& recipient);
        SSizeType write(const DatagramPacket& message);

        bool joinIpv4MulticastGroupByIfAddr(const StringType& multicastAddr, const StringType& ifAddr);
        bool joinIpv4MulticastGroupByIfIndex(const StringType& multicastAddr, const StringType& inf);
        bool joinIpv6MulticastGroup(const StringType& multicastAddr, const StringType& inf);

        bool leaveIpv4MulticastGroupByIfAddr(const StringType& multicastAddr, const StringType& ifAddr);
        bool leaveIpv4MulticastGroupByIfIndex(const StringType& multicastAddr, const StringType& inf);
        bool leaveIpv6MulticastGroup(const StringType& multicastAddr, const StringType& inf);

    private:
        DatagramChannelRawPtr channel_;
    };
} // namespace nets

#endif // NETS_DATAGRAM_CHANNEL_CONTEXT_H
