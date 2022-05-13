//
// Created by n021949 on 2022/5/13.
//

#include "nets/net/core/Socket.h"

namespace nets::net
{
	InetAddress::InetAddress(const ::std::string& addr, bool ipv6)
	{
		if (ipv6)
		{
			strToInet6(addr, &addr6_);
		}
		else
		{
			strToInet4(addr, &addr4_);
		}
	}

	InetAddress::InetAddress(const char* ip, uint16_t port, bool ipv6)
	{
		if (ipv6)
		{
			ipPortToInet6(ip, port, &addr6_);
		}
		else
		{
			ipPortToInet4(ip, port, &addr4_);
		}
	}

	InetAddress::InetAddress(const Ipv4Addr& addr4) : addr4_(addr4) {}

	InetAddress::InetAddress(const Ipv6Addr& addr6) : addr6_(addr6) {}
} // namespace nets::net