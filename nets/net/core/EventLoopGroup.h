//
// Created by guang19 on 2022/5/25.
//

#ifndef NETS_EVENTLOOPGROUP_H
#define NETS_EVENTLOOPGROUP_H

#include "nets/base/concurrency/ThreadPool.h"
#include "nets/net/core/EventLoop.h"

namespace nets::net
{
	class EventLoopGroup
	{
	public:
		using ThreadPoolPtr = ::std::unique_ptr<nets::base::ThreadPool>;
	};
} // namespace nets::net

#endif // NETS_EVENTLOOPGROUP_H
