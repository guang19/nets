//
// Created by n021949 on 2022/5/13.
//

#ifndef NETS_INETUTIL_H
#define NETS_INETUTIL_H

#include <cstdint>
#include <netinet/in.h>
#include <string>

namespace nets::net
{
	using IpType = uint32_t;
	using PortType = uint16_t;
	using Ipv4Addr = struct sockaddr_in;
	using Ipv6Addr = struct sockaddr_in6;

	void setLoopBackInet4Address(PortType port, Ipv4Addr* addr);
	void setLoopBackInet6Address(PortType port, Ipv6Addr* addr);

	void setAnyInet4Address(PortType port, Ipv4Addr* addr);
	void setAnyInet6Address(PortType port, Ipv6Addr* addr);

	void ipPortToInet4Address(const char* ip, PortType port, Ipv4Addr* addr);
	void ipPortToInet6Address(const char* ip, PortType port, Ipv6Addr* addr);
}; // namespace nets::net

#endif // NETS_INETUTIL_H
