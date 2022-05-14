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
	using PortType = uint16_t;
	using SockAddr = struct sockaddr;
	using Ipv4Addr = struct sockaddr_in;
	using Ipv6Addr = struct sockaddr_in6;

	PortType netToHost16(PortType netPort);
	PortType hostToNet16(PortType hostPort);

	void setLoopBackInet4Address(PortType port, Ipv4Addr* addr);
	void setLoopBackInet6Address(PortType port, Ipv6Addr* addr);

	void setAnyInet4Address(PortType port, Ipv4Addr* addr);
	void setAnyInet6Address(PortType port, Ipv6Addr* addr);

	void ipPortToInet4Address(const char* ip, PortType port, Ipv4Addr* addr);
	void ipPortToInet6Address(const char* ip, PortType port, Ipv6Addr* addr);

	void getSockAddressIp(const SockAddr* sockAddr, char* buffer, ::size_t len);
}; // namespace nets::net

#endif // NETS_INETUTIL_H
