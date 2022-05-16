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

	}

	void EpollSelector::addChannel(Selector::ChannelPtr channel) {}

	void EpollSelector::updateChannel(Selector::ChannelPtr channel) {}

	void EpollSelector::removeChannel(Selector::ChannelPtr channel) {}
} // namespace nets::net