//
// Created by guang19 on 2022/5/15.
//

#ifndef NETS_CHANNEL_H
#define NETS_CHANNEL_H

#include <cstdint>
#include <memory>
#include <sys/epoll.h>

#include "nets/net/core/Socket.h"

namespace nets::net
{

	class EventLoop;

	enum EventType
	{
		None = 0,
		ReadEvent = EPOLLIN,
		PriReadEvent = EPOLLPRI,
		WriteEvent = EPOLLOUT,
		ErrorEvent = EPOLLERR,
	};

	class Channel : public Socket, public ::std::enable_shared_from_this<Channel>
	{
	public:
		using EventLoopPtr = ::std::shared_ptr<EventLoop>;
		using ChannelPtr = ::std::shared_ptr<Channel>;

	public:
		explicit Channel(EventLoopPtr eventLoop);
		~Channel() override = default;

	public:
		void registerTo();
		void unregister();
		void modify();

	public:
		void addReadEvent();
		void removeReadEvent();
		void addWriteEvent();
		void removeWriteEvent();
		void resetEvent();

		inline int32_t events() const
		{
			return events_;
		}

		inline void setReadyEvent(EventType event)
		{
			readyEvents_ |= event;
		}

		inline bool isRegistered() const
		{
			return isRegistered_;
		}

		inline EventLoopPtr eventLoop() const
		{
			return eventLoop_;
		}

	private:
		void addEvent(EventType event);
		void removeEvent(EventType event);

	protected:
		int32_t events_ {EventType::None};
		int32_t readyEvents_ {EventType::None};
		bool isRegistered_ {false};
		EventLoopPtr eventLoop_ {nullptr};
	};
} // namespace nets::net

#endif // NETS_CHANNEL_H