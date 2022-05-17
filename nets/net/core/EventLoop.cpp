//
// Created by guang19 on 2022/5/15.
//

#include "nets/net/core/EventLoop.h"

#include "nets/net/core/Channel.h"
#include "nets/net/selector/Selector.h"

namespace nets::net
{
	EventLoop::EventLoop() : running_(false), selector_(SelectorFactory::getSelector())
	{

	}

	EventLoop::~EventLoop()
	{

	}

	void EventLoop::loop()
	{

	}

	void EventLoop::shutdown()
	{

	}

	void EventLoop::addChannel(ChannelPtr channel)
	{

	}

	void EventLoop::updateChannel(ChannelPtr channel)
	{

	}

	void EventLoop::removeChannel(ChannelPtr channel)
	{

	}
} // namespace nets::net