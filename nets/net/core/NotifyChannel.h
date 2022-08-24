//
// Created by guang19
//

#ifndef NETS_NET_NOTIFY_CHANNEL_H
#define NETS_NET_NOTIFY_CHANNEL_H

#include "nets/net/core/Channel.h"

namespace nets::net
{
    class NotifyChannel : public Channel
    {
    public:
        explicit NotifyChannel(EventLoopRawPtr eventLoop);
        ~NotifyChannel() override;

    public:
        FdType fd() const override;
        void notify() const;

    protected:
        void handleReadEvent() override;

    private:
        FdType eventFd_ {socket::InvalidFd};
    };
} // namespace nets::net

#endif // NETS_NET_NOTIFY_CHANNEL_H