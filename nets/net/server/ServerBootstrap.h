//
// Created by guang19 on 2022/5/18.
//

#ifndef NETS_SERVERBOOTSTRAP_H
#define NETS_SERVERBOOTSTRAP_H

#include "nets/base/concurrency/ThreadPool.h"
#include "nets/net/core/EventLoop.h"

namespace nets::net
{
	class ServerBootstrap : nets::base::Noncopyable
	{
	public:
		using ThreadPoolPtr = ::std::unique_ptr<nets::base::ThreadPool>;

	public:
		EventLoopPtr eventLoop() const
		{
			return mainEventLoop_;
		}

	private:
		::std::atomic_bool running_ {false};
		EventLoopPtr mainEventLoop_;
		ThreadPoolPtr threadPool_;
	};
} // namespace nets::net

#endif // NETS_SERVERBOOTSTRAP_H
