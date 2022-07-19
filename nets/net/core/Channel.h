//
// Created by guang19 on 2022/5/15.
//

#ifndef NETS_NET_CHANNEL_H
#define NETS_NET_CHANNEL_H

#include <memory>
#include <sys/epoll.h>

#include "ChannelContext.h"
#include "nets/base/Noncopyable.h"
#include "nets/net/core/Socket.h"

namespace nets::net
{
    class EventLoop;

    namespace
    {
        using EventType = uint32_t;
        // event type
        constexpr EventType ENoneEvent = 0;
        constexpr EventType EReadEvent = 0x01;
        constexpr EventType EWriteEvent = 0x01 << 1;
        constexpr EventType EErrorEvent = 0x01 << 2;
    } // namespace

    class Channel : nets::base::Noncopyable, public ::std::enable_shared_from_this<Channel>
    {
    public:
        using EventLoopRawPtr = EventLoop*;

    public:
        explicit Channel(EventLoopRawPtr eventLoop);
        virtual ~Channel() = default;

    public:
        bool registerTo();
        bool modify();
        void deregister();

    public:
        virtual FdType fd() const = 0;

        virtual ChannelContext* channelContext();
        virtual ChannelHandlerPipeline* channelHandlerPipeline();

        inline EventLoopRawPtr eventLoop() const
        {
            return eventLoop_;
        }

        inline EventType events() const
        {
            return events_;
        }

        inline bool isNoneEvent() const
        {
            return events_ == ENoneEvent;
        }

        inline void setEvents(EventType events)
        {
            events_ = events;
        }

        inline void addEvent(EventType event)
        {
            events_ |= event;
        }

        inline bool isRegistered() const
        {
            return isRegistered_;
        }

        inline void setRegistered(bool registered)
        {
            isRegistered_ = registered;
        }

        inline void setReadyEvents(EventType events)
        {
            readyEvents_ = events;
        }

        inline void addReadyEvent(EventType event)
        {
            readyEvents_ |= event;
        }

    public:
        void handleEvent();
        virtual void handleErrorEvent();
        virtual void handleReadEvent();
        virtual void handleWriteEvent();

    protected:
        // channel unique identifier per EventLoop thread
        EventType events_ {ENoneEvent};
        EventType readyEvents_ {ENoneEvent};
        bool isRegistered_ {false};
        EventLoopRawPtr eventLoop_ {nullptr};
    };
} // namespace nets::net

#endif // NETS_NET_CHANNEL_H