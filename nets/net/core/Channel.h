//
// Created by guang19 on 2022/5/15.
//

#ifndef NETS_CHANNEL_H
#define NETS_CHANNEL_H

#include <memory>
#include <sys/epoll.h>

#include "nets/net/core/Socket.h"

namespace nets::net
{

	class EventLoop;

	namespace
	{
		using EventType = uint32_t;
		// event type
		EventType NoneEvent = 0;
		EventType ReadEvent = EPOLLIN | EPOLLPRI;
		EventType WriteEvent = EPOLLOUT;
		EventType ErrorEvent = EPOLLERR;
	} // namespace

	class Channel : public Socket, public ::std::enable_shared_from_this<Channel>
	{
	public:
		using EventLoopPtr = ::std::shared_ptr<EventLoop>;

	public:
		explicit Channel(EventLoopPtr eventLoop);
		~Channel() override = default;

	public:
		void registerTo();
		void modify();
		void unregister();

	public:
		void addReadEvent();
		void removeReadEvent();
		void addWriteEvent();
		void removeWriteEvent();
		void resetEvent();
		void setReadyEvent(EventType event);

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

		inline EventLoopPtr eventLoop() const
		{
			return eventLoop_;
		}

	private:
		void addEvent(EventType event);
		void removeEvent(EventType event);

	protected:
		EventType events_ {NoneEvent};
		EventType readyEvents_ {NoneEvent};
		bool isRegistered_ {false};
		EventLoopPtr eventLoop_ {nullptr};
	};
} // namespace nets::net

#endif // NETS_CHANNEL_H