//
// Created by guang19 on 2022/5/15.
//

#ifndef NETS_NET_CHANNEL_H
#define NETS_NET_CHANNEL_H

#include <memory>
#include <sys/epoll.h>

#include "nets/base/Noncopyable.h"
#include "nets/net/core/ChannelHandlerPipeline.h"
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
		constexpr EventType EWriteEvent = 0x02;
		constexpr EventType EErrorEvent = 0x03;
	} // namespace

	class Channel : nets::base::Noncopyable, public ::std::enable_shared_from_this<Channel>
	{
	public:
		using EventLoopRawPtr = EventLoop*;

	public:
		explicit Channel(EventLoopRawPtr eventLoop);
		virtual ~Channel() = default;

	public:
		void registerTo();
		void modify();
		void deregister();

	public:
		inline EventLoopRawPtr eventLoop() const
		{
			return eventLoop_;
		}

		virtual FdType fd() const = 0;

		inline EventType events() const
		{
			return events_;
		}

		inline bool isNoneEvent() const
		{
			return events_ == ENoneEvent;
		}

		inline bool setEvents(EventType events)
		{
			return events_ == events;
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
		ChannelHandlerPipeline& pipeline()
		{
			return channelHandlerPipeline_;
		}

	public:
		void handleEvent();
		virtual void handleReadEvent() = 0;
		virtual void handleWriteEvent() = 0;
		virtual void handleErrorEvent() = 0;

	protected:
		// channel unique identifier per EventLoop thread
		EventType events_ {ENoneEvent};
		EventType readyEvents_ {ENoneEvent};
		bool isRegistered_ {false};
		ChannelHandlerPipeline channelHandlerPipeline_ {};
		EventLoopRawPtr eventLoop_ {nullptr};
	};
} // namespace nets::net

#endif // NETS_NET_CHANNEL_H