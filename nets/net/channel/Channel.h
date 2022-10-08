//
// Created by guang19
//

#ifndef NETS_CHANNEL_H
#define NETS_CHANNEL_H

#include <memory>
#include <unordered_map>

#include "nets/base/Noncopyable.h"
#include "nets/net/core/ChannelOption.h"
#include "nets/net/core/Socket.h"

namespace nets
{
    class EventLoop;

    namespace
    {
        using EventType = ::uint32_t;
        // event type
        constexpr EventType gNoneEvent = 0;
        constexpr EventType gReadEvent = 0x01;
        constexpr EventType gWriteEvent = 0x01 << 1;
        constexpr EventType gErrorEvent = 0x01 << 2;
    } // namespace

    class Channel : Noncopyable, public ::std::enable_shared_from_this<Channel>
    {
    public:
        using StringType = std::string;
        using EventLoopRawPtr = EventLoop*;
        using ChannelOptionList = ::std::unordered_map<SockOption, ChannelOption::ValueType>;

    public:
        explicit Channel(EventLoopRawPtr eventLoop);
        virtual ~Channel() = default;

    public:
        bool registerTo();
        bool modify();
        void deregister();

    public:
        EventLoopRawPtr eventLoop() const;
        EventType events() const;
        void setEvents(EventType events);
        void addEvent(EventType event);
        bool isNoneEvent() const;
        bool hasReadEvent() const;
        bool hasWriteEvent() const;

        void setReadyEvents(EventType events);
        void addReadyEvent(EventType event);

        bool isRegistered() const;
        void setRegistered(bool registered);

    public:
        void setChannelOption(SockOption sockOption, const ChannelOption::ValueType& value);
        virtual void setBacklog(::int32_t backlog);

    public:
        virtual FdType fd() const = 0;
        void handleEvent();

    protected:
        virtual void handleReadEvent();
        virtual void handleWriteEvent();
        virtual void handleErrorEvent();

    protected:
        // channel unique identifier per EventLoop thread
        EventType events_;
        EventType readyEvents_;
        bool isRegistered_;
        EventLoopRawPtr eventLoop_;
    };
} // namespace nets

#endif // NETS_CHANNEL_H