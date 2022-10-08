//
// Created by guang19
//

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