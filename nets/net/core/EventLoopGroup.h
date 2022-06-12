//
// Created by guang19 on 2022/5/25.
//

#ifndef NETS_NET_EVENT_LOOP_GROUP_H
#define NETS_NET_EVENT_LOOP_GROUP_H

#include "nets/base/concurrency/ThreadPool.h"
#include "nets/net/core/EventLoop.h"

namespace nets::net
{
	class EventLoopGroup
	{
	public:
		using ThreadPoolType = nets::base::ThreadPool;
		using ThreadPoolPtr = ::std::unique_ptr<ThreadPoolType>;
		using EventLoopRawPtr = EventLoop*;
		using EventLoopPtr = ::std::unique_ptr<EventLoop>;
		using EventLoopList = ::std::vector<EventLoopPtr>;
		using SizeType = typename EventLoopList::size_type;
		using MutexType = ::std::mutex;
		using LockGuardType = ::std::lock_guard<MutexType>;

	public:
		explicit EventLoopGroup(nets::base::ThreadPool::NType numOfEventLoops);
		~EventLoopGroup() = default;

	public:
		void loopEach();
		EventLoopRawPtr next();

	private:
		::std::atomic_bool started_ {false};
		uint32_t nextLoop_ {0};
		uint32_t numOfEventLoops_ {0};
		EventLoopList eventLoops_ {};
		ThreadPoolPtr eventLoopThreadPool_ {nullptr};
		MutexType mutex_ {};
	};
} // namespace nets::net

#endif // NETS_NET_EVENT_LOOP_GROUP_H
