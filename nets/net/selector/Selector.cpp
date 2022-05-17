//
// Created by n021949 on 2022/5/16.
//

#include "nets/net/selector/Selector.h"

#include "nets/net/selector/EpollSelector.h"

namespace nets::net
{
	bool Selector::hasChannel(const ChannelPtr& channel)
	{
		const auto it = channels_.find(channel->fd());
		return it != channels_.end() && it->second == channel;
	}

	Selector* SelectorFactory::getSelector()
	{
		return new EpollSelector(nullptr);
	}
}; // namespace nets::net