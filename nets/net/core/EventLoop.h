//
// Created by guang19 on 2022/5/15.
//

#ifndef NETS_EVENTLOOP_H
#define NETS_EVENTLOOP_H

#include <atomic>
#include <memory>

#include "nets/base/Noncopyable.h"

namespace nets::net
{
	class EventLoop : base::Noncopyable
	{
	public:
		using EventLoopPtr = ::std::shared_ptr<EventLoop>;

	public:
		void loop();
		void exit();
	private:
		::std::atomic_bool running_ {false};
	};
} // namespace nets::net

#endif // NETS_EVENTLOOP_H
