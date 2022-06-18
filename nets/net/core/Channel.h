//
// Created by guang19 on 2022/5/15.
//

#ifndef NETS_NET_CHANNEL_H
#define NETS_NET_CHANNEL_H

#include <memory>
#include <sys/epoll.h>

#include "nets/base/Noncopyable.h"
#include "nets/net/core/ChannelHandler.h"
#include "nets/net/core/Socket.h"

namespace nets::net
{
	class EventLoop;

	namespace
	{
		using EventType = uint32_t;
		// event type
		constexpr EventType NoneEvent = 0;
		constexpr EventType ReadEvent = EPOLLIN | EPOLLPRI;
		constexpr EventType WriteEvent = EPOLLOUT;
		constexpr EventType ErrorEvent = EPOLLERR;
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
		virtual FdType fd() const = 0;

		inline EventType events() const
		{
			return events_;
		}

		inline bool isNoneEvent() const
		{
			return events_ == NoneEvent;
		}

		inline bool isRegistered() const
		{
			return isRegistered_;
		}

		inline void setRegistered(bool registered)
		{
			isRegistered_ = registered;
		}

		inline bool isReadable() const
		{
			return events_ & ReadEvent;
		}

		inline bool isWritable() const
		{
			return events_ & WriteEvent;
		}

		inline EventLoopRawPtr eventLoop() const
		{
			return eventLoop_;
		}

		void addReadEvent();
		void removeReadEvent();
		void addWriteEvent();
		void removeWriteEvent();
		void resetEvent();
		void setReadyEvent(EventType event);

	private:
		void addEvent(EventType event);
		void removeEvent(EventType event);

	protected:
		// channel unique identifier per EventLoop thread
		EventType events_ {NoneEvent};
		EventType readyEvents_ {NoneEvent};
		bool isRegistered_ {false};
		EventLoopRawPtr eventLoop_ {nullptr};
	};
} // namespace nets::net

#endif // NETS_NET_CHANNEL_H