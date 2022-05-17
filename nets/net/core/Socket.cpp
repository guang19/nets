//
// Created by n021949 on 2022/5/13.
//

#include "nets/net/core/Socket.h"

#include "nets/base/CommonMacro.h"

namespace nets::net
{
	InetAddress::InetAddress(const SockAddr4& addr4) : addr4_(addr4) {}

	InetAddress::InetAddress(const SockAddr6& addr6) : addr6_(addr6) {}

	InetAddress::InetAddress(const char* ip, PortType port, bool ipv4)
	{
		if (ipv4)
		{
			MEMZERO(&addr4_, sizeof(SockAddr4));
			util::ipPortToInet4Addr(ip, port, &addr4_);
		}
		else
		{
			MEMZERO(&addr6_, sizeof(SockAddr6));
			util::ipPortToInet6Addr(ip, port, &addr6_);
		}
	}

	InetAddress InetAddress::createAnyInetAddress(PortType port, bool ipv4)
	{
		if (ipv4)
		{
			SockAddr4 addr {};
			MEMZERO(&addr, sizeof(SockAddr4));
			util::createAnyInet4Addr(port, &addr);
			return InetAddress(addr);
		}
		else
		{
			SockAddr6 addr {};
			MEMZERO(&addr, sizeof(SockAddr6));
			util::createAnyInet6Addr(port, &addr);
			return InetAddress(addr);
		}
	}

	InetAddress InetAddress::createLoopBackInetAddress(PortType port, bool ipv4)
	{
		if (ipv4)
		{
			SockAddr4 addr {};
			MEMZERO(&addr, sizeof(SockAddr4));
			util::createLoopBackInet4Addr(port, &addr);
			return InetAddress(addr);
		}
		else
		{
			SockAddr6 addr {};
			MEMZERO(&addr, sizeof(SockAddr6));
			util::createLoopBackInet6Addr(port, &addr);
			return InetAddress(addr);
		}
	}
	::std::string InetAddress::ip() const
	{
		char buffer[64] = {0};
		util::getIpFromSockAddr(&addr_, buffer, static_cast<SockLenType>(sizeof(buffer)));
		return buffer;
	}

	PortType InetAddress::port() const
	{
		return util::netToHost16(addr4_.sin_port);
	}

	::std::string InetAddress::toString() const
	{
		char buffer[64] = {0};
		util::sockAddrToString(&addr_, buffer, sizeof(buffer));
		return buffer;
	}

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

	void Socket::bind(const InetAddress& addr)
	{
		util::bind(sockFd_, addr.cSockAddr());
	}

	void Socket::listen()
	{
		util::listen(sockFd_);
	}

	FdType Socket::accept(InetAddress* addr)
	{
		return util::accept(sockFd_, addr->sockAddr());
	}
} // namespace nets::net