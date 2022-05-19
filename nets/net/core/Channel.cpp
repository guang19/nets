//
// Created by guang19 on 2022/5/15.
//

#include "nets/net/core/Channel.h"

#include "nets/net/core/EventLoop.h"

namespace nets::net
{
	Channel::Channel(EventLoopPtr eventLoop)
		: events_(EventType::NoneEvent), isRegistered_(false), eventLoop_(::std::move(eventLoop))
	{
	}

	void Channel::registerTo()
	{
		eventLoop_->registerChannel(shared_from_this());
	}

	void Channel::modify()
	{
		eventLoop_->modifyChannel(shared_from_this());
	}

	void Channel::unregister()
	{
		eventLoop_->unregisterChannel(shared_from_this());
	}

	void Channel::addReadEvent()
	{
		addEvent(EventType::ReadEvent);
	}

	void Channel::removeReadEvent()
	{
		removeEvent(EventType::ReadEvent);
	}

	void Channel::addWriteEvent()
	{
		addEvent(EventType::WriteEvent);
	}

	void Channel::removeWriteEvent()
	{
		removeEvent(EventType::WriteEvent);
	}

	void Channel::resetEvent()
	{
		events_ = EventType::NoneEvent;
	}

	void Channel::setReadyEvent(EventType event)
	{
		readyEvents_ = event;
	}

	void Channel::addEvent(EventType event)
	{
		events_ |= event;
	}

	void Channel::removeEvent(EventType event)
	{
		events_ &= ~event;
	}
} // namespace nets::net