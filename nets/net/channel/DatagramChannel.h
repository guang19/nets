//
// Created by guang19
//

#ifndef NETS_NET_DATAGRAM_CHANNEL_H
#define NETS_NET_DATAGRAM_CHANNEL_H

#include "nets/net/channel/Channel.h"
#include "nets/net/core/DatagramChannelHandlerPipeline.h"

namespace nets::net
{
    class DatagramChannel : public Channel
    {
    public:
        using StringType = ::std::string;

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

        inline void setChannelOptions(const ChannelOptionList& channelOptions)
        {
            channelOptions_.insert(channelOptions_.end(), channelOptions.begin(), channelOptions.end());
        }

        void bind(const InetSockAddress& localAddress);

        void write(const void* message, SizeType length, const InetSockAddress& recipient);
        void write(const StringType& message, const InetSockAddress& recipient);
        void write(const ByteBuffer& message, const InetSockAddress& recipient);
        void write(const DatagramPacket& message);

    protected:
        void handleReadEvent() override;
        void handleWriteEvent() override;
        void handleErrorEvent() override;

    private:
        void doWrite(const void* data, SizeType length, const InetSockAddress& recipient);
        void doWriteDirectly(const void* data, SizeType length, const InetSockAddress& recipient);

    private:
        FdType sockFd_ {socket::InvalidFd};
        using DatagramPacketList = ::std::vector<DatagramPacket>;
        DatagramPacketList writeBuffer_ {};

        DatagramChannelHandlerPipeline channelHandlerPipeline_ {nullptr};
        ChannelOptionList channelOptions_ {};
    };
} // namespace nets::net

#endif // NETS_NET_DATAGRAM_CHANNEL_H