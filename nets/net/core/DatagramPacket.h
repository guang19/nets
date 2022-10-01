//
// Created by guang19
//

#ifndef NETS_DATAGRAM_PACKET_H
#define NETS_DATAGRAM_PACKET_H

#include "nets/net/core/ByteBuffer.h"

namespace nets
{
    class DatagramPacket : public Copyable
    {
    public:
        DatagramPacket(const ByteBuffer& data, const InetSockAddress& recipient);
        DatagramPacket(ByteBuffer&& data, InetSockAddress&& recipient);
        ~DatagramPacket() = default;

        DatagramPacket(const DatagramPacket& other);
        DatagramPacket(DatagramPacket&& other) noexcept;
        DatagramPacket& operator=(const DatagramPacket& other);
        DatagramPacket& operator=(DatagramPacket&& other) noexcept;

    public:
        inline ByteBuffer byteBuffer()
        {
            return data_;
        }

        inline const ByteBuffer& byteBuffer() const
        {
            return data_;
        }

        inline const char* data()
        {
            return data_.data();
        }

        inline const char* data() const
        {
            return data_.data();
        }

        inline ByteBuffer::SizeType length() const
        {
            return data_.readableBytes();
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
} // namespace nets

#endif // NETS_DATAGRAM_PACKET_H