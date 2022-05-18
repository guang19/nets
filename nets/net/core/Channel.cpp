//
// Created by guang19 on 2022/5/15.
//

#include "nets/net/core/Channel.h"

namespace nets::net
{
	Channel::Channel(EventLoopPtr eventLoop, FdType fd) : socket_(fd), eventLoop_(::std::move(eventLoop))
	{
	}

	void Channel::addChannel(ChannelPtr channel)
	{
		if (eventLoop_->hasChannel(channel))
		{
		}
	}

	void Channel::updateChannel(ChannelPtr channel)
	{

	}

	void Channel::removeChannel(ChannelPtr channel)
	{

	}
} // namespace nets::net