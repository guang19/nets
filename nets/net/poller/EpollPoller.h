// MIT License
//
// Copyright (c) 2022 guang19
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

// @brief Poller implementation based on epoll

#ifndef NETS_EPOLL_POLLER_H
#define NETS_EPOLL_POLLER_H

#include <sys/epoll.h>
#include <vector>

#include "nets/net/poller/Poller.h"

namespace nets
{
    class EpollPoller : public Poller
    {
    private:
        using EpollEvent = struct epoll_event;
        using EventList = ::std::vector<EpollEvent>;

    public:
        explicit EpollPoller(EventLoopRawPtr eventLoop);
        ~EpollPoller() override;

    public:
        void poll(TimeType timeoutMs, ChannelList& activeChannels) override;
        bool registerChannel(ChannelRawPtr channel) override;
        bool modifyChannel(ChannelRawPtr channel) override;
        bool deregisterChannel(ChannelRawPtr channel) override;

    private:
        void prepareChannelReadyEvents(Int32Type numOfReadyEvents, ChannelList& activeChannels);
        bool epollCtl(Int32Type opt, ChannelRawPtr channel);
        const char* epollOptToString(Int32Type opt);

    private:
        FdType epollFd_;
        // dynamically growing array of events
        EventList events_;
    };
}; // namespace nets

#endif // NETS_EPOLL_POLLER_H