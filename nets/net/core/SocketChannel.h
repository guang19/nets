//
// Created by n021949 on 2022/5/19.
//

#ifndef NETS_NET_SOCKET_CHANNEL_H
#define NETS_NET_SOCKET_CHANNEL_H

#include "nets/net/core/Channel.h"

namespace nets::net
{
	class SocketChannel : public Channel
	{
	public:
		explicit SocketChannel(EventLoopRawPtr eventLoop);
		~SocketChannel() override = default;
	};
} // namespace nets::net

#endif // NETS_NET_SOCKET_CHANNEL_H
