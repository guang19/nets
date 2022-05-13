//
// Created by n021949 on 2022/5/13.
//

#include "nets/net/core/InetUtil.h"

#include <arpa/inet.h>

namespace nets::net
{
	void setLoopBackInet4Address(PortType port, Ipv4Addr* addr)
	{
		addr->sin_family = AF_INET;
		addr->sin_port = ::htobe16(port);
		addr->sin_addr.s_addr = INADDR_LOOPBACK;
	}

	void setLoopBackInet6Address(PortType port, Ipv6Addr* addr)
	{
		addr->sin6_family = AF_INET6;
		addr->sin6_port = htobe16(port);
		addr->sin6_addr = in6addr_loopback;
	}

	void setAnyInet4Address(PortType port, Ipv4Addr* addr)
	{
		addr->sin_family = AF_INET;
		addr->sin_port = htobe16(port);
		addr->sin_addr.s_addr = INADDR_ANY;
	}

	void setAnyInet6Address(PortType port, Ipv6Addr* addr)
	{
		addr->sin6_family = AF_INET6;
		addr->sin6_port = htobe16(port);
		addr->sin6_addr = in6addr_any;
	}

	void ipPortToInet4Address(const char* ip, PortType port, Ipv4Addr* addr)
	{
		addr->sin_family = AF_INET;
		addr->sin_port = htobe16(port);
		if (::inet_pton(AF_INET, ip, &(addr->sin_addr)) <= 0)
		{
			::fprintf(stderr, "Error:inet_pton\n");
			::exit(1);
		}
	}

	void ipPortToInet6Address(const char* ip, PortType port, Ipv6Addr* addr)
	{
		addr->sin6_family = AF_INET6;
		addr->sin6_port = htobe16(port);
		if (::inet_pton(AF_INET6, ip, &addr->sin6_addr) <= 0)
		{
			::fprintf(stderr, "Error:inet_pton\n");
			::exit(1);
		}
	}
} // namespace nets::net