//
// Created by guang19
//

#ifndef NETS_NOTIFY_CHANNEL_H
#define NETS_NOTIFY_CHANNEL_H

#include "nets/net/channel/Channel.h"

namespace nets
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
        FdType eventFd_ {socket::gInvalidFd};
    };
} // namespace nets

#endif // NETS_NOTIFY_CHANNEL_H