//
// Created by n021949 on 2022/5/16.
//

#include "nets/net/poller/EpollPoller.h"

#include <cassert>
#include <unistd.h>

#include "nets/base/CommonMacro.h"
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

	void EpollPoller::registerChannel(ChannelPtr channel)
	{
		if (!channel->isRegistered())
		{
			assert(!hasChannel(channel));
			if (epollCtl(EPOLL_CTL_ADD, channel))
			{
				channels_[channel->sockFd()] = channel;
				channel->setRegistered(true);
			}
		}
		else
		{
			modifyChannel(::std::move(channel));
		}
	}

	void EpollPoller::modifyChannel(ChannelPtr channel)
	{
		if (channel->isRegistered())
		{
			assert(hasChannel(channel));
			if (channel->isNoneEvent())
			{
				if (epollCtl(EPOLL_CTL_DEL, channel))
				{
					channels_.erase(channel->sockFd());
					channel->setRegistered(false);
				}
			}
			else
			{
				epollCtl(EPOLL_CTL_MOD, channel);
			}
		}
		else
		{
			registerChannel(::std::move(channel));
		}
	}

	void EpollPoller::unregisterChannel(ChannelPtr channel)
	{
		assert(hasChannel(channel));
		assert(channel->isRegistered());
		assert(channel->isNoneEvent());
		epollCtl(EPOLL_CTL_DEL, channel);
		channel->setRegistered(false);
		channels_.erase(channel->sockFd());
	}

	bool EpollPoller::epollCtl(int32_t opt, const ChannelPtr& channel)
	{
		EpollEvent event {};
		FdType fd = channel->sockFd();
		event.data.fd = fd;
		event.data.ptr = channel.get();
		event.events = channel->events();
		if (::epoll_ctl(epollFd_, opt, fd, &event) == 0)
		{
			return true;
		}
		LOGS_ERROR << "epoll ctl " << epollOptToString(opt) << " failed";
		return false;
	}

	const char* EpollPoller::epollOptToString(int32_t opt)
	{
		switch (opt)
		{
			case EPOLL_CTL_ADD:
				return "add";
			case EPOLL_CTL_MOD:
				return "mod";
			case EPOLL_CTL_DEL:
				return "del";
			default:
				return "unknown operation";
		}
	}
} // namespace nets::net