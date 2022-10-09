//
// Created by guang19
//

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