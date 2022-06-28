//
// Created by guang19 on 2022/6/26.
//

#ifndef NETS_CHANNEL_HANDLER_PIPELINE_H
#define NETS_CHANNEL_HANDLER_PIPELINE_H

#include <list>
#include <memory>

#include "nets/base/Copyable.h"
#include "nets/net/core/ChannelHandler.h"

namespace nets::net
{
	class ChannelHandlerPipeline : nets::base::Copyable
	{
	public:
		using ChannelHandlerRawPtr = ChannelHandler*;
		using ChannelHandlerPtr = ::std::shared_ptr<ChannelHandler>;
		using ChannelHandlerList = ::std::list<ChannelHandlerPtr>;

	public:
		ChannelHandlerPipeline() = default;
		~ChannelHandlerPipeline() = default;

		ChannelHandlerPipeline(const ChannelHandlerPipeline& other);
		ChannelHandlerPipeline(ChannelHandlerPipeline&& other) noexcept;
		ChannelHandlerPipeline& operator=(const ChannelHandlerPipeline& other);
		ChannelHandlerPipeline& operator=(ChannelHandlerPipeline&& other) noexcept;

	public:
		void addFirst(ChannelHandlerRawPtr channelHandler);
		void addLast(ChannelHandlerRawPtr channelHandler);

	private:
		ChannelHandlerList channelHandlers_ {};
	};
} // namespace nets::net

#endif // NETS_CHANNEL_HANDLER_PIPELINE_H
