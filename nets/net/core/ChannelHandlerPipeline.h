//
// Created by guang19 on 2022/6/26.
//

#ifndef NETS_CHANNEL_HANDLER_PIPELINE_H
#define NETS_CHANNEL_HANDLER_PIPELINE_H

#include <list>
#include <memory>

#include "nets/net/core/ChannelHandler.h"

namespace nets::net
{
	class ChannelHandlerPipeline : nets::base::Noncopyable
	{
	public:
		using ChannelHandlerRawPtr = ChannelHandler*;
		using ChannelHandlerPtr = ::std::unique_ptr<ChannelHandler>;
		using ChannelHandlerList = ::std::list<ChannelHandlerPtr>;

	public:
		ChannelHandlerPipeline() = default;
		~ChannelHandlerPipeline() = default;

	public:
		void addFirst(ChannelHandlerRawPtr channelHandler);
		void addLast(ChannelHandlerRawPtr channelHandler);

	private:
		ChannelHandlerList channelHandlers_ {};
	};
} // namespace nets::net

#endif // NETS_CHANNEL_HANDLER_PIPELINE_H
