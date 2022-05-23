//
// Created by guang19 on 2022/5/15.
//

#ifndef NETS_CHANNEL_H
#define NETS_CHANNEL_H

#include <memory>
#include <sys/epoll.h>

#include "nets/base/Noncopyable.h"

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

	class Channel : nets::base::Noncopyable
	{
	public:
		using IdType = uint32_t;
		using FdType = int32_t;
		using EventLoopPtr = ::std::shared_ptr<EventLoop>;

	public:
		explicit Channel(EventLoopPtr eventLoop);
		~Channel() = default;

	public:
		void registerTo();
		void modify();
		void unregister();

	public:
		IdType uniqueId() const
		{
			return uniqueId_;
		}

		virtual FdType sockFd() const = 0;

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

		inline bool isReadable() const
		{
			return events_ & ReadEvent;
		}

		inline bool isWritable() const
		{
			return events_ & WriteEvent;
		}

		inline EventLoopPtr eventLoop() const
		{
			return eventLoop_;
		}

	private:
		void addEvent(EventType event);
		void removeEvent(EventType event);

	protected:
		IdType uniqueId_ {0};
		EventType events_ {NoneEvent};
		EventType readyEvents_ {NoneEvent};
		bool isRegistered_ {false};
		EventLoopPtr eventLoop_ {nullptr};
	};
} // namespace nets::net

#endif // NETS_CHANNEL_H