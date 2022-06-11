//
// Created by guang19 on 2022/5/25.
//

#include "nets/net/core/EventLoopGroup.h"

#include "nets/base/CommonMacro.h"

namespace nets::net
{
	EventLoopGroup::EventLoopGroup() : EventLoopGroup(0) {}

	EventLoopGroup::EventLoopGroup(nets::base::ThreadPool::NType numOfSubLoops)
	{
		if (numOfSubLoops > 0)
		{
			subLoops_.reserve(numOfSubLoops);
			eventLoopThreadPool_ = ::std::make_unique<nets::base::ThreadPool>(numOfSubLoops, numOfSubLoops, 0);
		}
	}

	EventLoopGroup::EventLoopRawPtr EventLoopGroup::next()
	{
		EventLoopRawPtr eventLoop = mainLoop_.get();
		if (!subLoops_.empty())
		{
			eventLoop = subLoops_[nextLoop_++].get();
			if (static_cast<SizeType>(nextLoop_) >= subLoops_.size())
			{
				nextLoop_ = 0;
			}
		}
		return eventLoop;
	}
} // namespace nets::net