//
// Created by guang19 on 2022/5/18.
//

#include "nets/net/server/ServerSocketChannel.h"

#include <cassert>

namespace nets::net
{
	ServerSocketChannel::ServerSocketChannel(EventLoopRawPtr eventLoop)
		: Channel(eventLoop), sockFd_(socket::InvalidFd), idleFd_(socket::createIdleFd())
	{
	}

	ServerSocketChannel::~ServerSocketChannel()
	{
		deregister();
		socket::closeFd(sockFd_);
		socket::closeFd(idleFd_);
	}

	void ServerSocketChannel::bind(const InetSockAddress& sockAddress)
	{
		socket::createTcpSocket(AF_INET);
		socket::setSockAddrReuse(sockFd_, true);
		socket::setSockPortReuse(sockFd_, true);
		socket::setSockNonBlock(sockFd_, true);
		socket::bind(sockFd_, sockAddress.csockAddr());
		socket::listen(sockFd_);
		addEvent(EReadEvent);
		registerTo();
		assert(sockFd_ > 0);
	}

	void ServerSocketChannel::handleReadEvent()
	{

	}

	void ServerSocketChannel::handleWriteEvent()
	{

	}

	void ServerSocketChannel::handleErrorEvent()
	{

	}
} // namespace nets::net