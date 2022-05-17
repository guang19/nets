//
// Created by guang19 on 2022/5/15.
//

#include "nets/net/core/EventLoop.h"

#include <cassert>

#include "nets/base/log/Logging.h"
#include "nets/base/ThreadHelper.h"
#include "nets/net/core/Channel.h"
#include "nets/net/selector/Selector.h"

namespace nets::net
{
	namespace
	{
		__thread EventLoop* CurrentEventLoop = nullptr;
	}

	EventLoop::EventLoop() : running_(false), selector_(SelectorFactory::getSelector()), currentTid_(base::currentTid())
	{
		assert(CurrentEventLoop == nullptr);
		// one loop per thread
		if (CurrentEventLoop != nullptr)
		{
			LOGS_FATAL << "there must be only one loop per thread";
		}
		else
		{
			CurrentEventLoop = this;
		}
		LOGS_INFO << "one loop is created in thread" << currentTid_;
	}

	EventLoop::~EventLoop() {}

	bool EventLoop::isInEventLoopThread() const
	{
		return this == CurrentEventLoop;
	}

	EventLoop::EventLoopPtr EventLoop::currentThreadEventLoop() const
	{
		assert(CurrentEventLoop != nullptr);
		return CurrentEventLoop->shared_from_this();
	}

	void EventLoop::loop() {}

	void EventLoop::shutdown() {}

	void EventLoop::addChannel(ChannelPtr channel)
	{
		selector_->addChannel(channel);
	}

	void EventLoop::updateChannel(ChannelPtr channel)
	{
		selector_->updateChannel(channel);
	}

	void EventLoop::removeChannel(ChannelPtr channel)
	{
		selector_->removeChannel(channel);
	}
} // namespace nets::net