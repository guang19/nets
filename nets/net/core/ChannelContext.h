//
// Created by guang19 on 2022/6/28.
//

#ifndef NETS_CHANNEL_CONTEXT_H
#define NETS_CHANNEL_CONTEXT_H

#include <memory>

#include "nets/base/Noncopyable.h"
#include "nets/net/core/ChannelHandlerPipeline.h"

namespace nets::net
{
	class Channel;

	class ChannelContext : nets::base::Noncopyable
	{
	public:
		using ChannelPtr = ::std::shared_ptr<Channel>;

	public:
		explicit ChannelContext(ChannelPtr channel);
		~ChannelContext() = default;

	public:
		inline ChannelPtr channel()
		{
			return channel_;
		}

		inline ChannelHandlerPipeline& pipeline()
		{
			return channelHandlerPipeline_;
		}

	private:
		ChannelPtr channel_;
		ChannelHandlerPipeline channelHandlerPipeline_{};
	};
}

#endif // NETS_CHANNEL_CONTEXT_H
