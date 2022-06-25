//
// Created by guang19 on 2022/5/23.
//

#ifndef NETS_NET_CHANNEL_HANDLER_H
#define NETS_NET_CHANNEL_HANDLER_H

#include "nets/base/Noncopyable.h"

namespace nets::net
{
	class ChannelHandler : nets::base::Noncopyable
	{
	public:
		ChannelHandler() = default;
		~ChannelHandler() = default;
	};
} // namespace nets::net

#endif // NETS_NET_CHANNEL_HANDLER_H
