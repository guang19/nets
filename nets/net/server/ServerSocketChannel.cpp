//
// Created by guang19 on 2022/5/18.
//

#include "nets/net/server/ServerSocketChannel.h"

namespace nets::net
{
	ServerSocketChannel::ServerSocketChannel(EventLoopPtr eventLoop) : Channel(::std::move(eventLoop)) {}

	void ServerSocketChannel::socket(bool ipv4)
	{
		sockFd_ = util::createTcpNonBlockSocket(ipv4 ? AF_INET : AF_INET6);
		setAddrReuse();
		setPortReuse();
	}
} // namespace nets::net