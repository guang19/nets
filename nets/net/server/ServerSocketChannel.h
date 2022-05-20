//
// Created by guang19 on 2022/5/18.
//

#ifndef NETS_SERVERSOCKETCHANNEL_H
#define NETS_SERVERSOCKETCHANNEL_H

#include "nets/net/core/Channel.h"

namespace nets::net
{
	class ServerSocketChannel : public Channel
	{
	public:
		explicit ServerSocketChannel(EventLoopPtr eventLoop, SockAddrFamily sockAddrFamily);
		~ServerSocketChannel() override = default;
	};
} // namespace nets::net

#endif // NETS_SERVERSOCKETCHANNEL_H
