//
// Created by guang19 on 2022/5/18.
//

#include "nets/net/server/ServerSocketChannel.h"

#include <cassert>

namespace nets::net
{
	ServerSocketChannel::ServerSocketChannel(EventLoopPtr eventLoop, SockAddrFamily sockAddrFamily)
		: Channel(eventLoop), sockFd_(socket::createTcpSocket(sockAddrFamily)), idleFd_(socket::createIdleFd())
	{
		socket::setSockAddrReuse(sockFd_, true);
		socket::setSockPortReuse(sockFd_, true);
	}

	ServerSocketChannel::~ServerSocketChannel()
	{
		socket::closeFd(sockFd_);
		socket::closeFd(idleFd_);
	}

	void ServerSocketChannel::bind(const InetSockAddress& sockAddress)
	{
		socket::bind(sockFd_, sockAddress.cSockAddr());
		socket::listen(sockFd_);
		addReadEvent();
		assert(isReadable());
		registerToLoop();
		assert(uniqueId() >= 0);
	}
} // namespace nets::net