//
// Created by guang19 on 2022/5/15.
//

#ifndef NETS_CHANNEL_H
#define NETS_CHANNEL_H

#include <cstdint>
#include <sys/epoll.h>

#include "nets/net/core/EventLoop.h"
#include "nets/net/core/Socket.h"

namespace nets::net
{
	enum EventType
	{
		None = 0,
		ReadEvent = EPOLLIN,
		PriReadEvent = EPOLLPRI,
		WriteEvent = EPOLLOUT,
		ErrorEvent = EPOLLERR,
	};

	class Channel : base::Noncopyable, public ::std::enable_shared_from_this<Channel>
	{
	public:
		using FdType = int32_t;
		using EventLoopPtr = ::std::shared_ptr<EventLoop>;
		using ChannelPtr = ::std::shared_ptr<Channel>;

	public:
		explicit Channel(EventLoopPtr eventLoop, FdType fd);
		virtual ~Channel() = default;

	public:
		void registerTo();
		void unregister();
		void modify();

		virtual void handleReadEvent() = 0;
		virtual void handleWriteEvent() = 0;
		virtual void handleCloseEvent() = 0;
		virtual void handleErrorEvent() = 0;

	public:
		inline FdType fd() const
		{
			return fd_;
		}

		inline int32_t events() const
		{
			return events_;
		}

		inline void setReadyEvents(int32_t readyEvents)
		{
			readyEvents_ = readyEvents;
		}

		inline bool isRegistered() const
		{
			return isRegistered_;
		}

		inline void setRegistered(bool isRegistered)
		{
			isRegistered_ = isRegistered;
		}

		inline EventLoopPtr eventLoop() const
		{
			return eventLoop_;
		}

	protected:
		FdType fd_ {-1};
		int32_t events_ {EventType::None};
		int32_t readyEvents_ {EventType::None};
		::std::atomic_bool isRegistered_ {false};
		EventLoopPtr eventLoop_ {nullptr};
	};
} // namespace nets::net

#endif // NETS_CHANNEL_H