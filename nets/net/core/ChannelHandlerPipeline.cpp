//
// Created by guang19 on 2022/6/26.
//

#include "nets/net/core/ChannelHandlerPipeline.h"

namespace nets::net
{
	void ChannelHandlerPipeline::addFirst(ChannelHandlerRawPtr channelHandler)
	{
		channelHandlers_.push_front(::std::unique_ptr<ChannelHandler>(channelHandler));
	}

	void ChannelHandlerPipeline::addLast(ChannelHandlerRawPtr channelHandler)
	{
		channelHandlers_.push_back(::std::unique_ptr<ChannelHandler>(channelHandler));
	}
}