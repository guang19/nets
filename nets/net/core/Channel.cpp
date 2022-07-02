//
// Created by guang19 on 2022/5/15.
//

#include "nets/net/core/Channel.h"

#include "nets/net/core/EventLoop.h"

namespace nets::net
{
	Channel::Channel()
		: events_(ENoneEvent), readyEvents_(ENoneEvent), isRegistered_(false), channelContext_(shared_from_this()),
		  eventLoop_(nullptr)
	{
	}

	void Channel::registerTo(EventLoopRawPtr eventLoop)
	{
		eventLoop_ = eventLoop;
		eventLoop_->registerChannel(shared_from_this());
	}

	void Channel::modify()
	{
		eventLoop_->modifyChannel(shared_from_this());
	}

	void Channel::deregister()
	{
		eventLoop_->deregisterChannel(shared_from_this());
		eventLoop_ = nullptr;
	}

	void Channel::handleEvent()
	{
		if (readyEvents_ & EErrorEvent)
		{
			handleErrorEvent();
		}
		if (readyEvents_ & EReadEvent)
		{
			handleReadEvent();
		}
		if (readyEvents_ & EWriteEvent)
		{
			handleWriteEvent();
		}
	}
} // namespace nets::net