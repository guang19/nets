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
		__thread EventLoop* CurrentEventLoop = nullptr;
	}

	EventLoop::EventLoop() : running_(false), poller_(PollerFactory::getPoller()), currentTid_(base::currentTid())
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