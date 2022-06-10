//
// Created by guang19 on 2022/5/25.
//

#include "nets/net/core/EventLoopGroup.h"

#include "nets/base/CommonMacro.h"

namespace nets::net
{
	namespace
	{
		const uint32_t DefaultEventLoopThreads = AVAILABLE_PROCESSOR << 1;
	}

	EventLoopGroup::EventLoopGroup(nets::base::ThreadPool::NType numOfSubLoops)
	{
		numOfSubLoops = numOfSubLoops <= 0 ? AVAILABLE_PROCESSOR : numOfSubLoops;
		eventLoops_.reserve(numOfSubLoops);
		eventLoopThreadPool_ = ::std::make_unique<nets::base::ThreadPool>(numOfSubLoops, numOfSubLoops, 0);
	}

	EventLoopGroup::EventLoopRawPtr EventLoopGroup::next()
	{
		return nullptr;
	}
}