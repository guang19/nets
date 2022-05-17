//
// Created by n021949 on 2022/5/16.
//

#include "nets/net/selector/EpollSelector.h"

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

	EpollSelector::EpollSelector() : Selector(nullptr)
	{

	}

	EpollSelector::EpollSelector(EventLoopPtr eventLoop)
		: Selector(::std::move(eventLoop)), epollFd_(::epoll_create1(EPOLL_CLOEXEC)), events_(InitEventSize)
	{
		assert(epollFd_ >= 0);
		if (epollFd_ < 0)
		{
			LOGS_FATAL << "epoll create1 failed with result epollFd < 0";
		}
	}

	EpollSelector::~EpollSelector()
	{
		::close(epollFd_);
	}

	void EpollSelector::select()
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

	void EpollSelector::addChannel(Selector::ChannelPtr channel) {}

	void EpollSelector::updateChannel(Selector::ChannelPtr channel) {}

	void EpollSelector::removeChannel(Selector::ChannelPtr channel) {}
} // namespace nets::net