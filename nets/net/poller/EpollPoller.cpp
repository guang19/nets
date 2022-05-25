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
		// unique id per-thread
		__thread Channel::IdType ChannelUniqueId = 0;
	} // namespace

	EpollPoller::EpollPoller(EventLoopPtr eventLoop)
		: Poller(eventLoop), epollFd_(::epoll_create1(EPOLL_CLOEXEC)), events_(InitEventSize)
	{
		assert(epollFd_ >= 0);
		if (epollFd_ < 0)
		{
			LOGS_FATAL << "epoll create1 failed with result epollFd < 0";
		}
	}

	EpollPoller::~EpollPoller()
	{
		socket::closeFd(epollFd_);
	}

	void EpollPoller::poll(int32_t timeoutMs, ChannelList activeChannels)
	{
		SizeType size = events_.size();
		int32_t numOfReadyEvent = ::epoll_wait(epollFd_, &*events_.begin(), static_cast<int32_t>(size), timeoutMs);
		if (numOfReadyEvent > 0)
		{
			LOGS_DEBUG << "epoll wait:" << numOfReadyEvent << " events";
			prepareChannelEvents(numOfReadyEvent, activeChannels);
			if (static_cast<EventList::size_type>(numOfReadyEvent) == size)
			{
				events_.resize(size + (size >> 1));
			}
		}
		else if (numOfReadyEvent < 0)
		{
			LOGS_ERROR << "epoll error";
		}
	}

	void EpollPoller::prepareChannelEvents(int32_t numOfReadyEvent, ChannelList activeChannels)
	{
		for (int32_t i = 0; i< numOfReadyEvent; ++i)
		{
			auto channel = static_cast<ChannelRawPtr>(events_[i].data.ptr);
			channel->setReadyEvent(events_[i].events);
		}
	}

	void EpollPoller::registerChannel(ChannelPtr channel)
	{
		if (!channel->isRegistered())
		{
			assert(!hasChannel(channel));
			if (epollCtl(EPOLL_CTL_ADD, channel))
			{
				++ChannelUniqueId;
				assert(ChannelUniqueId > 0);
				// ChannelUniqueId is an unsigned type, ChannelUniqueId equals 0 means that ChannelUniqueId has reached the maximum
				if (ChannelUniqueId == 0)
				{
					++ChannelUniqueId;
				}
				channels_[ChannelUniqueId] = channel;
				channel->setRegistered(true);
			}
		}
		else
		{
			modifyChannel(channel);
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
					channels_.erase(channel->uniqueId());
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
			registerChannel(channel);
		}
	}

	void EpollPoller::deregisterChannel(ChannelPtr channel)
	{
		assert(hasChannel(channel));
		assert(channel->isRegistered());
		assert(channel->isNoneEvent());
		epollCtl(EPOLL_CTL_DEL, channel);
		channel->setRegistered(false);
		channels_.erase(channel->uniqueId());
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