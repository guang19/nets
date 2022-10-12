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

// @brief The Poller polls registered Channel events

#ifndef NETS_POLLER_H
#define NETS_POLLER_H

#include <unordered_map>
#include <vector>

#include "nets/base/Noncopyable.h"
#include "nets/net/channel/Channel.h"

namespace nets
{
    class Channel;

    class Poller : Noncopyable
    {
    public:
        using TimeType = ::time_t;
        using ChannelRawPtr = Channel*;
        using ChannelList = ::std::vector<ChannelRawPtr>;
        using EventLoopRawPtr = EventLoop*;

    public:
        explicit Poller(EventLoopRawPtr eventLoop) : eventLoop_(eventLoop) {}
        virtual ~Poller() = default;

    public:
        virtual void poll(TimeType timeoutMs, ChannelList& activeChannels) = 0;
        virtual bool registerChannel(ChannelRawPtr channel) = 0;
        virtual bool modifyChannel(ChannelRawPtr channel) = 0;
        virtual bool deregisterChannel(ChannelRawPtr channel) = 0;

        inline EventLoopRawPtr eventLoop() const
        {
            return eventLoop_;
        }

    protected:
        EventLoopRawPtr eventLoop_;
    };

    class PollerFactory
    {
    public:
        using PollerPtr = ::std::unique_ptr<Poller>;
        using EventLoopRawPtr = Poller::EventLoopRawPtr;

    public:
        static PollerPtr getPoller(EventLoopRawPtr eventLoop);
    };
}; // namespace nets

#endif // NETS_POLLER_H