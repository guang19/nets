//
// Created by guang19
//

#include "nets/net/core/DatagramPacket.h"

namespace nets
{
    DatagramPacket::DatagramPacket(const ByteBuffer& data, const InetSockAddress& recipient)
        : data_(data), recipient_(recipient)
    {
    }

    DatagramPacket::DatagramPacket(ByteBuffer&& data, InetSockAddress&& recipient)
        : data_(::std::move(data)), recipient_(::std::move(recipient))
    {
    }

    DatagramPacket::DatagramPacket(const DatagramPacket& other) : DatagramPacket(other.data_, other.recipient_) {}

    DatagramPacket::DatagramPacket(DatagramPacket&& other) noexcept
        : data_(::std::move(other.data_)), recipient_(::std::move(other.recipient_))
    {
    }

    DatagramPacket& DatagramPacket::operator=(const DatagramPacket& other)
    {
        if (this != &other)
        {
            data_ = other.data_;
            recipient_ = other.recipient_;
        }
        return *this;
    }

    DatagramPacket& DatagramPacket::operator=(DatagramPacket&& other) noexcept
    {
        if (this != &other)
        {
            data_ = ::std::move(other.data_);
            recipient_ = ::std::move(other.recipient_);
        }
        return *this;
    }
} // namespace nets