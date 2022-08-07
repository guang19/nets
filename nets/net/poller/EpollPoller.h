//
// Created by guang19 on 2022/5/16.
//

#ifndef NETS_NET_EPOLL_POLLER_H
#define NETS_NET_EPOLL_POLLER_H

#include <vector>
#include <sys/epoll.h>

#include "nets/net/poller/Poller.h"

namespace nets::net
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
        void poll(int32_t timeoutMs, ChannelList& activeChannels) override;
        bool registerChannel(ChannelRawPtr channel) override;
        bool modifyChannel(ChannelRawPtr channel) override;
        bool deregisterChannel(ChannelRawPtr channel) override;

    private:
        void prepareChannelReadyEvents(int32_t numOfReadyEvent, ChannelList& activeChannels);
        bool epollCtl(int32_t opt, ChannelRawPtr channel);
        const char* epollOptToString(int32_t opt);

    private:
        FdType epollFd_ {-1};
        // dynamically growing array of events
        EventList events_ {};
    };
}; // namespace nets::net

#endif // NETS_NET_EPOLL_POLLER_H
