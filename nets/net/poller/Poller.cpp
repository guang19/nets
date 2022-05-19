//
// Created by n021949 on 2022/5/16.
//

#include "nets/net/poller/Poller.h"

#include "nets/net/poller/EpollPoller.h"

namespace nets::net
{
	bool Poller::hasChannel(ChannelPtr channel)
	{
		const auto it = channels_.find(channel->sockFd());
		return it != channels_.end() && it->second == channel;
	}

	Poller* PollerFactory::getPoller()
	{
		return new EpollPoller(nullptr);
	}
}; // namespace nets::net