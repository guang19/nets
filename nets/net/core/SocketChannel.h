//
// Created by guang19 on 2022/5/19.
//

#ifndef NETS_NET_SOCKET_CHANNEL_H
#define NETS_NET_SOCKET_CHANNEL_H

#include "nets/net/core/Channel.h"

namespace nets::net
{
    class SocketChannel : public Channel
    {
    public:
        explicit SocketChannel(EventLoopRawPtr eventLoop);
        ~SocketChannel() override = default;

    public:
        FdType fd() const override;

        void handleReadEvent() override;

        void handleWriteEvent() override;

        void handleErrorEvent() override;

    private:
        FdType fd_ {socket::InvalidFd};
    };
} // namespace nets::net

#endif // NETS_NET_SOCKET_CHANNEL_H
