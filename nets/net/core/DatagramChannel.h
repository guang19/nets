//
// Created by guang19
//

#ifndef NETS_NET_DATAGRAM_CHANNEL_H
#define NETS_NET_DATAGRAM_CHANNEL_H

#include "nets/net/core/Channel.h"

namespace nets::net
{
    class DatagramChannel : public Channel
    {
    public:
        explicit DatagramChannel(FdType sockFd, EventLoopRawPtr eventLoop);
        ~DatagramChannel() override;

    public:
        FdType fd() const override;

    protected:
        void handleReadEvent() override;
        void handleWriteEvent() override;
        void handleErrorEvent() override;

    private:
        FdType sockFd_ {socket::InvalidFd};
    };
} // namespace nets::net

#endif // NETS_NET_DATAGRAM_CHANNEL_H