//
// Created by guang19 on 2022/5/23.
//

#ifndef NETS_CHANNELHANDLER_H
#define NETS_CHANNELHANDLER_H

#include "nets/base/Noncopyable.h"

namespace nets::net
{
	class ChannelHandler : nets::base::Noncopyable
	{
	public:
		ChannelHandler() = default;
		~ChannelHandler() = default;

	public:
		virtual void handleReadEvent() = 0;
		virtual void handleWriteEvent() = 0;
		virtual void handleErrorEvent() = 0;
	};
} // namespace nets::net

#endif // NETS_CHANNELHANDLER_H
