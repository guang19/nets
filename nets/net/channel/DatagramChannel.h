//
// Created by guang19
//

#ifndef NETS_NET_DATAGRAM_CHANNEL_H
#define NETS_NET_DATAGRAM_CHANNEL_H

#include <vector>

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
            channelOptions_.insert(channelOptions.begin(), channelOptions.end());
        }

        void bind(const InetSockAddress& localAddress);

        // return the number of bytes sent. Otherwise, -1 shall be returned and errno set to indicate the error.
        SSizeType write(const void* message, SizeType length, const InetSockAddress& recipient);
        SSizeType write(const StringType& message, const InetSockAddress& recipient);
        SSizeType write(const ByteBuffer& message, const InetSockAddress& recipient);
        SSizeType write(const DatagramPacket& message);

    protected:
        void handleReadEvent() override;
        void handleErrorEvent() override;

    private:
        SSizeType doWrite(const void* data, SizeType length, const InetSockAddress& recipient);
        void handleReadError(::int32_t errNum);

    private:
        FdType sockFd_ {socket::gInvalidFd};
        using DatagramPacketList = ::std::vector<DatagramPacket>;

        DatagramChannelHandlerPipeline channelHandlerPipeline_ {nullptr};
        ChannelOptionList channelOptions_ {};
    };
} // namespace nets::net

#endif // NETS_NET_DATAGRAM_CHANNEL_H