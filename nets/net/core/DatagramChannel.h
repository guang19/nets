//
// Created by guang19
//

#ifndef NETS_NET_DATAGRAM_CHANNEL_H
#define NETS_NET_DATAGRAM_CHANNEL_H

#include "nets/net/core/Channel.h"
#include "nets/net/core/ChannelHandlerPipeline.h"

namespace nets::net
{
    class DatagramChannel : public Channel
    {
    public:
        explicit DatagramChannel(EventLoopRawPtr eventLoop);
        ~DatagramChannel() override;

    public:
        FdType fd() const override;

    public:
        inline ChannelHandlerPipeline& pipeline()
        {
            return channelHandlerPipeline_;
        }

        inline void setChannelOptions(const ChannelOptionList& channelOptions)
        {
            channelOptions_.insert(channelOptions_.end(), channelOptions.begin(), channelOptions.end());
        }

        void bind(const InetSockAddress& localAddress);

    protected:
        void handleReadEvent() override;
        void handleWriteEvent() override;
        void handleErrorEvent() override;

    private:
        FdType sockFd_ {socket::InvalidFd};
        ChannelHandlerPipeline channelHandlerPipeline_ {nullptr};
        ChannelOptionList channelOptions_ {};
    };
} // namespace nets::net

#endif // NETS_NET_DATAGRAM_CHANNEL_H