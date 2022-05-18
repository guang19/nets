//
// Created by guang19 on 2022/5/19.
//

#include "nets/net/server/ServerSocket.h"

namespace nets::net
{
	ServerSocket::ServerSocket(const InetAddress& addr) : Socket(util::createTcpNonBlockSocket(addr.ipFamily())), addr_(addr)
	{
	}

	ServerSocket::ServerSocket(const char* ip, PortType port, bool ipv4) : ServerSocket(InetAddress(ip, port, ipv4)) {}

	ServerSocket::~ServerSocket() {}

	void ServerSocket::bind()
	{
		util::bind(sockFd_, addr_.cSockAddr());
	}

	void ServerSocket::listen()
	{
		util::listen(sockFd_);
	}

	FdType ServerSocket::accept(InetAddress* addr)
	{
		return util::accept(sockFd_, addr->sockAddr());
	}
} // namespace nets::net