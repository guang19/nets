//
// Created by guang19 on 2022/5/18.
//

#include "nets/net/server/ServerSocketChannel.h"

namespace nets::net
{
	ServerSocketChannel::ServerSocketChannel(EventLoopPtr eventLoop, SockAddrFamily sockAddrFamily) : Channel(::std::move(eventLoop))
	{
		sockFd_ = util::createTcpNonBlockSocket(sockAddrFamily);
		setAddrReuse();
		setPortReuse();
	}
} // namespace nets::net