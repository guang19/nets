//
// Created by guang19 on 2022/5/15.
//

#ifndef NETS_NET_CHANNEL_H
#define NETS_NET_CHANNEL_H

#include <memory>
#include <sys/epoll.h>
#include <vector>

#include "ChannelContext.h"
#include "nets/base/Noncopyable.h"
#include "nets/net/core/ChannelOption.h"
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
        using ChannelOptionList = ::std::vector<ChannelOption>;
        using ChannelContextRawPtr = ChannelContext*;
        using ChannelHandlerPipelineRawPtr = ChannelHandlerPipeline*;

    public:
        explicit Channel(EventLoopRawPtr eventLoop);
        virtual ~Channel() = default;

    public:
        bool registerTo();
        bool modify();
        void deregister();

    public:
        virtual FdType fd() const = 0;
        virtual ChannelContextRawPtr channelContext();
        virtual ChannelHandlerPipelineRawPtr channelHandlerPipeline();
        virtual void setChannelOptions(const ChannelOptionList& channelOptions);

        EventLoopRawPtr eventLoop() const;

        EventType events() const;
        bool isNoneEvent() const;
        void setEvents(EventType events);
        void addEvent(EventType event);

        bool isRegistered() const;
        void setRegistered(bool registered);

        void setReadyEvents(EventType events);
        void addReadyEvent(EventType event);

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