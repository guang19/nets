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

// @brief The holder of the DatagramChannel

#include "nets/net/core/DatagramChannelContext.h"

#include "nets/net/channel/DatagramChannel.h"

namespace nets
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
} // namespace nets