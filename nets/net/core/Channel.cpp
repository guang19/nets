//
// Created by guang19 on 2022/5/15.
//

#include "nets/net/core/Channel.h"

namespace nets::net
{
	Channel::Channel(EventLoopPtr eventLoop, FdType fd)
		: fd_(fd), events_(EventType::None), isRegistered_(false), eventLoop_(::std::move(eventLoop))
	{
	}

	void Channel::registerTo()
	{
		eventLoop_->registerChannel(shared_from_this());
	}

	void Channel::unregister()
	{
		eventLoop_->unregisterChannel(shared_from_this());
	}

	void Channel::modify()
	{
		eventLoop_->modifyChannel(shared_from_this());
	}

} // namespace nets::net