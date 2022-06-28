//
// Created by guang19 on 2022/6/28.
//

#ifndef NETS_CHANNEL_CONTEXT_H
#define NETS_CHANNEL_CONTEXT_H

#include "nets/net/core/Channel.h"

namespace nets::net
{
	class ChannelContext : nets::base::Noncopyable
	{
	public:
		using ChannelPtr = ::std::shared_ptr<Channel>;

	public:
		explicit ChannelContext(ChannelPtr channel);

	public:
		ChannelPtr channel()
		{
			return channel_;
		}

		ChannelHandlerPipeline& pipeline()
		{
			return channel_->pipeline();
		}

	private:
		ChannelPtr channel_;
	};
}

#endif // NETS_CHANNEL_CONTEXT_H
