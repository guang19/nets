//
// Created by guang19 on 2022/5/15.
//

#include "nets/net/core/EventLoop.h"

#include <cassert>

#include "nets/base/log/Logging.h"
#include "nets/net/poller/Poller.h"

namespace nets::net
{
	namespace
	{
		__thread EventLoop* CurrentThreadEventLoop = nullptr;
	}

	EventLoop::EventLoop() : running_(false), threadId_(nets::base::currentTid()), poller_(PollerFactory::getPoller())
	{
		assert(CurrentThreadEventLoop == nullptr);
		// one loop per thread
		if (CurrentThreadEventLoop != nullptr)
		{
			LOGS_FATAL << "there must be only one loop per thread";
		}
		else
		{
			CurrentThreadEventLoop = this;
		}
		LOGS_INFO << "one loop is created in thread" << threadId_;
	}

	EventLoop::~EventLoop() {}

	void EventLoop::loop() {}

	void EventLoop::shutdown() {}

	bool EventLoop::isInCurrentEventLoop() const
	{
		return (this == CurrentThreadEventLoop);
	}

	EventLoop::EventLoopRawPtr EventLoop::currentThreadEventLoop() const
	{
		assert(isInCurrentEventLoop());
		return CurrentThreadEventLoop;
	}

	void EventLoop::notify() {}

	void EventLoop::registerChannel(ChannelPtr channel)
	{
		poller_->registerChannel(channel);
	}

	void EventLoop::modifyChannel(ChannelPtr channel)
	{
		poller_->modifyChannel(channel);
	}

	void EventLoop::deregisterChannel(ChannelPtr channel)
	{
		poller_->deregisterChannel(channel);
	}

	bool EventLoop::hasChannel(ChannelPtr channel)
	{
		return poller_->hasChannel(channel);
	}
} // namespace nets::net