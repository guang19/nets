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

// @brief Udp datagram

#ifndef NETS_DATAGRAM_PACKET_H
#define NETS_DATAGRAM_PACKET_H

#include "nets/net/core/ByteBuffer.h"

namespace nets
{
    class DatagramPacket : public Copyable
    {
    public:
        explicit DatagramPacket(const ByteBuffer& data, const InetSockAddress& recipient);
        explicit DatagramPacket(ByteBuffer&& data, InetSockAddress&& recipient);
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

        inline SizeType length() const
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
        ByteBuffer data_;
        InetSockAddress recipient_;
    };
} // namespace nets

#endif // NETS_DATAGRAM_PACKET_H