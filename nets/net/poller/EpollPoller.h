//
// Created by guang19
//

#ifndef NETS_EPOLL_POLLER_H
#define NETS_EPOLL_POLLER_H

#include <sys/epoll.h>
#include <vector>

#include "nets/net/poller/Poller.h"

namespace nets
{
    class EpollPoller : public Poller
    {
    public:
        using EpollEvent = struct epoll_event;
        using EventList = ::std::vector<EpollEvent>;
        using SizeType = EventList::size_type;

    public:
        explicit EpollPoller(EventLoopRawPtr eventLoop);
        ~EpollPoller() override;

    public:
        void poll(TimeType timeoutMs, ChannelList& activeChannels) override;
        bool registerChannel(ChannelRawPtr channel) override;
        bool modifyChannel(ChannelRawPtr channel) override;
        bool deregisterChannel(ChannelRawPtr channel) override;

    private:
        void prepareChannelReadyEvents(::int32_t numOfReadyEvents, ChannelList& activeChannels);
        bool epollCtl(::int32_t opt, ChannelRawPtr channel);
        const char* epollOptToString(::int32_t opt);

    private:
        FdType epollFd_;
        // dynamically growing array of events
        EventList events_;
    };
}; // namespace nets

#endif // NETS_EPOLL_POLLER_H