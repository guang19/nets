//
// Created by n021949 on 2022/5/13.
//

#include "nets/net/core/Socket.h"

#include "nets/base/CommonMacro.h"

namespace nets::net
{
	InetAddress::InetAddress(const Ipv4Addr& addr4) : addr4_(addr4) {}

	InetAddress::InetAddress(const Ipv6Addr& addr6) : addr6_(addr6) {}

	InetAddress::InetAddress(const char* ip, PortType port, bool ipv4)
	{
		if (ipv4)
		{
			MEMZERO(&addr4_, sizeof(Ipv4Addr));
			ipPortToInet4Addr(ip, port, &addr4_);
		}
		else
		{
			MEMZERO(&addr6_, sizeof(Ipv6Addr));
			ipPortToInet6Addr(ip, port, &addr6_);
		}
	}

	InetAddress InetAddress::createAnyInetAddress(PortType port, bool ipv4)
	{
		if (ipv4)
		{
			Ipv4Addr addr {};
			MEMZERO(&addr, sizeof(Ipv4Addr));
			createAnyInet4Addr(port, &addr);
			return InetAddress(addr);
		}
		else
		{
			Ipv6Addr addr {};
			MEMZERO(&addr, sizeof(Ipv6Addr));
			createAnyInet6Addr(port, &addr);
			return InetAddress(addr);
		}
	}

	InetAddress InetAddress::createLoopBackInetAddress(PortType port, bool ipv4)
	{
		if (ipv4)
		{
			Ipv4Addr addr {};
			MEMZERO(&addr, sizeof(Ipv4Addr));
			createLoopBackInet4Addr(port, &addr);
			return InetAddress(addr);
		}
		else
		{
			Ipv6Addr addr {};
			MEMZERO(&addr, sizeof(Ipv6Addr));
			createLoopBackInet6Addr(port, &addr);
			return InetAddress(addr);
		}
	}
	::std::string InetAddress::ip() const
	{
		char buffer[64] = {0};
		getIpFromSockAddr(&addr_, buffer, static_cast<SockLenType>(sizeof(buffer)));
		return buffer;
	}

	PortType InetAddress::port() const
	{
		return netToHost16(addr4_.sin_port);
	}

	::std::string InetAddress::toString() const
	{
		char buffer[64] = {0};
		sockAddrToString(&addr_, buffer, sizeof(buffer));
		return buffer;
	}

	void Socket::setSendBuf(SockLenType sendBufLen)
	{
		setSockSendBuf(sockFd_, sendBufLen);
	}

	void Socket::setRecvBuf(SockLenType recvBufLen)
	{
		setSockRecvBuf(sockFd_, recvBufLen);
	}

	void Socket::setAddrReuse(bool enable)
	{
		setSockAddrReuse(sockFd_, enable);
	}

	void Socket::setPortReuse(bool enable)
	{
		setSockPortReuse(sockFd_, enable);
	}

	void Socket::setKeepAlive(bool enable)
	{
		setSockKeepAlive(sockFd_, enable);
	}

	void Socket::setTcpNoDelay(bool enable)
	{
		setIpTcpNoDelay(sockFd_, enable);
	}

	void Socket::setNonBlock(bool enable)
	{
		setSockNonBlock(sockFd_, enable);
	}
} // namespace nets::net