//
// Created by n021949 on 2022/5/13.
//

#include "nets/net/core/Socket.h"

#include "nets/base/CommonMacro.h"

namespace nets::net
{
	Socket::Socket(FdType sockFd) : sockFd_(sockFd) {}

	Socket::~Socket()
	{
		util::closeSocket(sockFd_);
	}

	void Socket::setSendBuf(SockLenType sendBufLen)
	{
		util::setSockSendBuf(sockFd_, sendBufLen);
	}

	void Socket::setRecvBuf(SockLenType recvBufLen)
	{
		util::setSockRecvBuf(sockFd_, recvBufLen);
	}

	void Socket::setAddrReuse(bool enable)
	{
		util::setSockAddrReuse(sockFd_, enable);
	}

	void Socket::setPortReuse(bool enable)
	{
		util::setSockPortReuse(sockFd_, enable);
	}

	void Socket::setKeepAlive(bool enable)
	{
		util::setSockKeepAlive(sockFd_, enable);
	}

	void Socket::setTcpNoDelay(bool enable)
	{
		util::setIpTcpNoDelay(sockFd_, enable);
	}

	void Socket::setNonBlock(bool enable)
	{
		util::setSockNonBlock(sockFd_, enable);
	}
} // namespace nets::net