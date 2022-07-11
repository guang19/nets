//
// Created by guang19 on 2022/6/18.
//

#include "nets/net/core/NotifyChannel.h"

#include <sys/eventfd.h>

#include "nets/base/log/Logging.h"

namespace nets::net
{
	NotifyChannel::NotifyChannel(EventLoopRawPtr eventLoop)
		: Channel(eventLoop), eventFd_(::eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK))
	{
		if (eventFd_ < 0)
		{
			LOGS_FATAL << "NotifyChannel::NotifyChannel create eventFd failed";
		}
	}

	NotifyChannel::~NotifyChannel()
	{
		socket::closeFd(eventFd_);
	}

	FdType NotifyChannel::fd() const
	{
		return eventFd_;
	}

	void NotifyChannel::notify() const
	{
		uint64_t i = 1;
		::ssize_t n = socket::write(eventFd_, &i, sizeof(i));
		if (n != sizeof(i))
		{
			LOGS_ERROR << "NotifyChannel::notify failed";
		}
	}

	void NotifyChannel::handleReadEvent()
	{
		uint64_t i = 0;
		::ssize_t n = socket::read(eventFd_, &i, sizeof(i));
		if (n != sizeof(i))
		{
			LOGS_ERROR << "NotifyChannel::read failed";
		}
	}
} // namespace nets::net