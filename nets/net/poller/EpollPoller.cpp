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
		constexpr ::size_t EpollEventInitialSize = 12;
	} // namespace

	EpollPoller::EpollPoller(EventLoopRawPtr eventLoop)
		: Poller(eventLoop), epollFd_(::epoll_create1(EPOLL_CLOEXEC)), events_(EpollEventInitialSize)
	{
		assert(epollFd_ >= 0);
		if (epollFd_ < 0)
		{
			LOGS_FATAL << "EpollPoller::EpollPoller epoll create1 failed with result epollFd < 0";
		}
	}

	EpollPoller::~EpollPoller()
	{
		socket::closeFd(epollFd_);
	}

	void EpollPoller::poll(int32_t timeoutMs, ChannelList& activeChannels)
	{
		SizeType size = events_.size();
		int32_t numOfReadyEvent = ::epoll_wait(epollFd_, &*events_.begin(), static_cast<int32_t>(size), timeoutMs);
		if (numOfReadyEvent > 0)
		{
			LOGS_DEBUG << "EpollPoller::epoll wait:" << numOfReadyEvent << " events";
			prepareChannelReadEvents(numOfReadyEvent, activeChannels);
			if (static_cast<EventList::size_type>(numOfReadyEvent) == size)
			{
				size = size + (size >> 1);
				if (size >= INT32_MAX)
				{
					size = INT32_MAX - 1;
				}
			}
			events_.resize(size);
		}
		else if (numOfReadyEvent < 0)
		{
			LOGS_ERROR << "EpollPoller::epoll failed";
		}
	}

	void EpollPoller::prepareChannelReadEvents(int32_t numOfReadyEvent, ChannelList& activeChannels)
	{
		for (int32_t i = 0; i < numOfReadyEvent; ++i)
		{
			auto channel = static_cast<ChannelRawPtr>(events_[i].data.ptr);
			uint32_t revents = events_[i].events;
			channel->setReadyEvents(ENoneEvent);
			if (revents & (EPOLLERR | EPOLLHUP))
			{
				channel->addReadyEvent(EErrorEvent);
			}
			if (revents & (EPOLLIN | EPOLLPRI))
			{
				channel->addReadyEvent(EReadEvent);
			}
			if (revents & EPOLLOUT)
			{
				channel->addReadyEvent(EWriteEvent);
			}
			activeChannels.push_back(channel);
		}
	}

	bool EpollPoller::registerChannel(ChannelRawPtr channel)
	{
		if (!channel->isRegistered())
		{
			if (epollCtl(EPOLL_CTL_ADD, channel))
			{
				channel->setRegistered(true);
				return true;
			}
			return false;
		}
		else
		{
			return modifyChannel(channel);
		}
	}

	bool EpollPoller::modifyChannel(ChannelRawPtr channel)
	{
		if (channel->isRegistered())
		{
			if (channel->isNoneEvent())
			{
				return deregisterChannel(channel);
			}
			else
			{
				return epollCtl(EPOLL_CTL_MOD, channel);
			}
		}
		else
		{
			return registerChannel(channel);
		}
	}

	bool EpollPoller::deregisterChannel(ChannelRawPtr channel)
	{
		channel->setRegistered(false);
		return epollCtl(EPOLL_CTL_DEL, channel);
	}

	bool EpollPoller::epollCtl(int32_t opt, const ChannelRawPtr channel)
	{
		EpollEvent event {};
		FdType fd = channel->fd();
		event.data.fd = fd;
		event.data.ptr = channel;
		event.events = ENoneEvent;
		EventType events = channel->events();
		if (events & EReadEvent)
		{
			event.events |= EPOLLIN;
		}
		if (events & EWriteEvent)
		{
			event.events |= EPOLLOUT;
		}
		if (0 == ::epoll_ctl(epollFd_, opt, fd, &event))
		{
			return true;
		}
		LOGS_ERROR << "EpollPoller::epollCtl [" << epollOptToString(opt) << "] failed";
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