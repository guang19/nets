//
// Created by guang19 on 2022/5/15.
//

#include "nets/net/core/EventLoop.h"

#include <cassert>
#include <utility>

#include "nets/base/log/Logging.h"
#include "nets/base/ThreadHelper.h"
#include "nets/net/poller/Poller.h"

namespace nets::net
{
	namespace
	{
		__thread EventLoop* CurrentThreadEventLoop = nullptr;
	}

	EventLoop::EventLoop() : running_(false), poller_(PollerFactory::getPoller())
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
		LOGS_INFO << "one loop is created in thread" << base::currentTid();
	}

	EventLoop::~EventLoop() {}

	void EventLoop::loop() {}

	void EventLoop::shutdown() {}

	bool EventLoop::isInEventLoopThread() const
	{
		return (this == CurrentThreadEventLoop);
	}

	EventLoop::EventLoopPtr EventLoop::currentThreadEventLoop() const
	{
		assert(CurrentThreadEventLoop != nullptr);
		return CurrentThreadEventLoop->shared_from_this();
	}

	void EventLoop::notify() {}

	void EventLoop::registerChannel(ChannelPtr channel)
	{
		poller_->registerChannel(std::move(channel));
	}

	void EventLoop::modifyChannel(ChannelPtr channel)
	{
		poller_->modifyChannel(std::move(channel));
	}

	void EventLoop::unregisterChannel(ChannelPtr channel)
	{
		poller_->unregisterChannel(std::move(channel));
	}

	bool EventLoop::hasChannel(ChannelPtr channel)
	{
		return poller_->hasChannel(std::move(channel));
	}
} // namespace nets::net