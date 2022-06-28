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
		int8_t c = 1;
		::ssize_t n = socket::write(eventFd_, &c, sizeof(c));
		if (n != sizeof(c))
		{
			LOGS_ERROR << "NotifyChannel::notify write failed";
		}
	}

	void NotifyChannel::handleReadEvent() {}
	void NotifyChannel::handleWriteEvent() {}
	void NotifyChannel::handleErrorEvent() {}
} // namespace nets::net