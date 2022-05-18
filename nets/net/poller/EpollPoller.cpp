//
// Created by n021949 on 2022/5/16.
//

#include "nets/net/poller/EpollPoller.h"

#include <cassert>
#include <unistd.h>

#include "nets/base/log/Logging.h"

namespace nets::net
{
	namespace
	{
		constexpr ::size_t InitEventSize = 12;
		constexpr ::time_t Timeout = 1;
	} // namespace

	EpollPoller::EpollPoller(EventLoopPtr eventLoop)
		: Poller(::std::move(eventLoop)), epollFd_(::epoll_create1(EPOLL_CLOEXEC)), events_(InitEventSize)
	{
		assert(epollFd_ >= 0);
		if (epollFd_ < 0)
		{
			LOGS_FATAL << "epoll create1 failed with result epollFd < 0";
		}
	}

	EpollPoller::~EpollPoller()
	{
		::close(epollFd_);
	}

	void EpollPoller::poll()
	{
		int32_t requestEvents = ::epoll_wait(epollFd_, &*events_.begin(), static_cast<int32_t>(events_.size()), Timeout);
		if (requestEvents > 0)
		{
			LOGS_INFO << "epoll:" << requestEvents << " events";
		}
		else if (requestEvents == 0)
		{
			LOGS_DEBUG << "epoll no events";
		}
		else
		{
			LOGS_ERROR << "epoll error";
		}
	}

	void EpollPoller::addChannel(Poller::ChannelPtr channel) {}

	void EpollPoller::updateChannel(Poller::ChannelPtr channel) {}

	void EpollPoller::removeChannel(Poller::ChannelPtr channel) {}
} // namespace nets::net