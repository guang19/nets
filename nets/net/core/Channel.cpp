//
// Created by guang19 on 2022/5/15.
//

#include "nets/net/core/Channel.h"

namespace nets::net
{
	Channel::Channel(EventLoopPtr eventLoop, FdType fd)
		: socket_(fd), events_(EventType::None), eventLoop_(::std::move(eventLoop))
	{
	}
} // namespace nets::net