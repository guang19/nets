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
		using TimeType = ::time_t;
		constexpr TimeType PollTimeoutMs = 10000;
		__thread EventLoop* CurrentThreadEventLoop = nullptr;
	}

	EventLoop::EventLoop()
		: running_(false), threadId_(nets::base::currentTid()), notifier_(::std::make_unique<NotifyChannel>(this)),
		  poller_(PollerFactory::getPoller())
	{
		assert(CurrentThreadEventLoop == nullptr);
		// one loop per thread
		if (CurrentThreadEventLoop != nullptr)
		{
			LOGS_FATAL << "EventLoop::EventLoop there must be only one loop per thread";
		}
		else
		{
			CurrentThreadEventLoop = this;
		}
		notifier_->addEvent(EReadEvent);
		registerChannel(notifier_);
		LOGS_INFO << "EventLoop::EventLoop one loop is created in thread" << threadId_;
	}

	EventLoop::~EventLoop() {}

	void EventLoop::run()
	{
		assert(!running_);
		running_ = true;
		while (running_)
		{
			activeChannels_.clear();
			poller_->poll(PollTimeoutMs, activeChannels_);
			for (auto& channel : activeChannels_)
			{
				channel->handleEvent();
			}
			runPendingTasks();
		}
		assert(!running_);
	}

	void EventLoop::runPendingTasks()
	{
		TaskList tmpTasks {};
		{
			LockGuardType lock(mutex_);
			tmpTasks.swap(pendingTasks_);
		}
		for (const auto& t : tmpTasks)
		{
			t();
		}
	}

	void EventLoop::shutdown() {}

	bool EventLoop::inCurrentEventLoop() const
	{
		return (this == CurrentThreadEventLoop);
	}

	EventLoop::EventLoopRawPtr EventLoop::currentEventLoop() const
	{
		assert(inCurrentEventLoop());
		return CurrentThreadEventLoop;
	}

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