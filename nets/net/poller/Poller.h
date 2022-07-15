//
// Created by guang19 on 2022/5/16.
//

#ifndef NETS_NET_POLLER_H
#define NETS_NET_POLLER_H

#include <unordered_map>
#include <vector>

#include "nets/base/Noncopyable.h"
#include "nets/net/core/Channel.h"

namespace nets::net
{
    class Channel;

    class Poller : nets::base::Noncopyable
    {
    public:
        using ChannelRawPtr = Channel*;
        using ChannelList = ::std::vector<ChannelRawPtr>;
        using EventLoopRawPtr = EventLoop*;

    public:
        explicit Poller(EventLoopRawPtr eventLoop) : eventLoop_(eventLoop) {}
        virtual ~Poller() = default;

    public:
        virtual void poll(int32_t timeoutMs, ChannelList& activeChannels) = 0;
        virtual bool registerChannel(ChannelRawPtr channel) = 0;
        virtual bool modifyChannel(ChannelRawPtr channel) = 0;
        virtual bool deregisterChannel(ChannelRawPtr channel) = 0;

        inline EventLoopRawPtr eventLoop() const
        {
            return eventLoop_;
        }

    protected:
        EventLoopRawPtr eventLoop_ {nullptr};
    };

    class PollerFactory
    {
    public:
        using PollerPtr = ::std::unique_ptr<Poller>;
        using EventLoopRawPtr = Poller::EventLoopRawPtr;

    public:
        static PollerPtr getPoller(EventLoopRawPtr eventLoop);
    };
}; // namespace nets::net

#endif // NETS_NET_POLLER_H
