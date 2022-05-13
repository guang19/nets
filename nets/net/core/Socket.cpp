//
// Created by n021949 on 2022/5/13.
//

#include "nets/net/core/Socket.h"

#include "nets/base/CommonMacro.h"

namespace nets::net
{
	InetAddress::InetAddress(const char* ip, PortType port, bool ipv6)
	{
		if (ipv6)
		{
			MEMZERO(&addr6_, sizeof(Ipv6Addr));
			ipPortToInet6Address(ip, port, &addr6_);
		}
		else
		{
			MEMZERO(&addr4_, sizeof(Ipv4Addr));
			ipPortToInet4Address(ip, port, &addr4_);
		}
	}

	InetAddress::InetAddress(const Ipv4Addr& addr4) : addr4_(addr4) {}

	InetAddress::InetAddress(const Ipv6Addr& addr6) : addr6_(addr6) {}

	InetAddress InetAddress::createAnyInetAddress(PortType port, bool ipv6)
	{
		if (ipv6)
		{
			Ipv6Addr addr {};
			MEMZERO(&addr, sizeof(Ipv6Addr));
			setAnyInet6Address(port, &addr);
			return InetAddress(addr);
		}
		else
		{
			Ipv4Addr addr {};
			MEMZERO(&addr, sizeof(Ipv4Addr));
			setAnyInet4Address(port, &addr);
			return InetAddress(addr);
		}
	}

	InetAddress InetAddress::createLoopBackInetAddress(PortType port, bool ipv6)
	{
		if (ipv6)
		{
			Ipv6Addr addr {};
			MEMZERO(&addr, sizeof(Ipv6Addr));
			setLoopBackInet6Address(port, &addr);
			return InetAddress(addr);
		}
		else
		{
			Ipv4Addr addr {};
			MEMZERO(&addr, sizeof(Ipv4Addr));
			setLoopBackInet4Address(port, &addr);
			return InetAddress(addr);
		}
	}
} // namespace nets::net