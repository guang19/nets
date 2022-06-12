//
// Created by guang19 on 2022/5/25.
//

#include "nets/net/core/EventLoopGroup.h"

namespace nets::net
{
	EventLoopGroup::EventLoopGroup(nets::base::ThreadPool::NType numOfEventLoops)
		: started_(false)
	{
		if (numOfEventLoops <= 0)
		{
			throw ::std::invalid_argument("numOfEventLoops must be greater than 0");
		}
		numOfEventLoops_ = numOfEventLoops;
		eventLoops_.reserve(numOfEventLoops_);
	}

	void EventLoopGroup::loopEach()
	{
		LockGuardType lock(mutex_);
		started_ = true;
		eventLoopThreadPool_ = ::std::make_unique<ThreadPoolType>(numOfEventLoops_, numOfEventLoops_, numOfEventLoops_);
		for (uint32_t i = 0; i < numOfEventLoops_; ++i)
		{
			auto eventLoop = new EventLoop();
			eventLoops_.emplace_back(eventLoop);
			eventLoopThreadPool_->execute([&eventLoop]()
										  {
											 eventLoop->loop();
										  });
		}
	}

	EventLoopGroup::EventLoopRawPtr EventLoopGroup::next()
	{
		EventLoopRawPtr nextEventLoop = eventLoops_[nextLoop_++].get();
		if (static_cast<SizeType>(nextLoop_) >= eventLoops_.size())
		{
			nextLoop_ = 0;
		}
		return nextEventLoop;
	}
} // namespace nets::net