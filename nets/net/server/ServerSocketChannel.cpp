//
// Created by guang19 on 2022/5/18.
//

#include "nets/net/server/ServerSocketChannel.h"

namespace nets::net
{
	ServerSocketChannel::ServerSocketChannel(EventLoopPtr eventLoop, SockAddrFamily sockAddrFamily)
		: Channel(eventLoop), sockFd_(socket::createTcpSocket(sockAddrFamily))
	{
		socket::setSockAddrReuse(sockFd_, true);
		socket::setSockPortReuse(sockFd_, true);
	}

	ServerSocketChannel::~ServerSocketChannel()
	{
		socket::closeSocket(sockFd_);
	}

	void ServerSocketChannel::bind(const InetSockAddress& sockAddress)
	{
		socket::bind(sockFd_, sockAddress.cSockAddr());
	}
} // namespace nets::net