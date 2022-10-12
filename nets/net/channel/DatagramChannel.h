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

// @brief The encapsulation of udp server or udp client operations

#ifndef NETS_DATAGRAM_CHANNEL_H
#define NETS_DATAGRAM_CHANNEL_H

#include <vector>

#include "nets/net/channel/Channel.h"
#include "nets/net/core/DatagramChannelHandlerPipeline.h"

namespace nets
{
    class DatagramChannel : public Channel
    {
    public:
        explicit DatagramChannel(EventLoopRawPtr eventLoop);
        ~DatagramChannel() override;

    public:
        FdType fd() const override;

    public:
        inline DatagramChannelHandlerPipeline& pipeline()
        {
            return channelHandlerPipeline_;
        }

        void setChannelOptions(const ChannelOptionList& channelOptions);

        void bind(const InetSockAddress& localAddress);

        // return the number of bytes sent. Otherwise, -1 shall be returned and errno set to indicate the error.
        SSizeType write(const void* message, SizeType length, const InetSockAddress& recipient);
        SSizeType write(const StringType& message, const InetSockAddress& recipient);
        SSizeType write(const ByteBuffer& message, const InetSockAddress& recipient);
        SSizeType write(const DatagramPacket& message);

        // join multicast group
        bool joinIpv4MulticastGroupByIfAddr(const StringType& multicastAddr, const StringType& ifAddr);
        bool joinIpv4MulticastGroupByIfIndex(const StringType& multicastAddr, const StringType& inf);
        bool joinIpv6MulticastGroup(const StringType& multicastAddr, const StringType& inf);

        // leave multicast group
        bool leaveIpv4MulticastGroupByIfAddr(const StringType& multicastAddr, const StringType& ifAddr);
        bool leaveIpv4MulticastGroupByIfIndex(const StringType& multicastAddr, const StringType& inf);
        bool leaveIpv6MulticastGroup(const StringType& multicastAddr, const StringType& inf);

    protected:
        void handleReadEvent() override;
        void handleErrorEvent() override;

    private:
        SSizeType doWrite(const void* data, SizeType length, const InetSockAddress& recipient);
        void handleReadError(::int32_t errNum);

    private:
        FdType sockFd_;
        DatagramChannelHandlerPipeline channelHandlerPipeline_;
        ChannelOptionList channelOptions_;
    };
} // namespace nets

#endif // NETS_DATAGRAM_CHANNEL_H