//
// Created by guang19
//

#ifndef NETS_NET_DATAGRAM_PACKET_H
#define NETS_NET_DATAGRAM_PACKET_H

#include "nets/net/core/ByteBuffer.h"

namespace nets::net
{
    class DatagramPacket : public nets::base::Copyable
    {
    public:
        DatagramPacket(const ByteBuffer& data, const InetSockAddress& recipient);
        ~DatagramPacket() = default;

        DatagramPacket(const DatagramPacket& other);
        DatagramPacket(DatagramPacket&& other) noexcept;
        DatagramPacket& operator=(const DatagramPacket& other);
        DatagramPacket& operator=(DatagramPacket&& other) noexcept;

    public:
        inline ByteBuffer data()
        {
            return data_;
        }

        inline const ByteBuffer& data() const
        {
            return data_;
        }

        inline InetSockAddress recipient()
        {
            return recipient_;
        }

        inline const InetSockAddress& recipient() const
        {
            return recipient_;
        }

    private:
        ByteBuffer data_ {};
        InetSockAddress recipient_ {};
    };
}

#endif // NETS_NET_DATAGRAM_PACKET_H