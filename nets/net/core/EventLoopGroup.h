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
		using EventLoopRawPtr = EventLoop*;
		using EventLoopPtr = ::std::unique_ptr<EventLoop>;
		using EventLoopList = ::std::vector<EventLoopPtr>;

	public:
		EventLoopGroup(nets::base::ThreadPool::SizeType numOfSubLoops);
		~EventLoopGroup() = default;

	public:
		EventLoopRawPtr next();

	private:
		EventLoopList eventLoops_ {};
		ThreadPoolPtr threadPool {nullptr};
	};
} // namespace nets::net

#endif // NETS_EVENTLOOPGROUP_H
