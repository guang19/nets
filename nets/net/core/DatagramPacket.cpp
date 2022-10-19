// MIT License
//
// Copyright (c) 2022 guang19
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

// @brief udp datagram

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