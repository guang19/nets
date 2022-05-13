//
// Created by n021949 on 2022/5/13.
//

#ifndef NETS_INETUTIL_H
#define NETS_INETUTIL_H

#include <netinet/in.h>
#include <cstdint>
#include <string>

namespace nets::net
{
	using PortType = uint16_t;
	using Ipv4Addr = struct sockaddr_in;
	using Ipv6Addr = struct sockaddr_in6;

	//like: "127.0.0.1:8080"
	void strToInet4(const ::std::string& str, Ipv4Addr* addr);
	//like: "[FF01:02::05]:8080"
	void strToInet6(const ::std::string& str, Ipv6Addr* addr);

	void ipPortToInet4(const char* ip, PortType port, Ipv4Addr* addr);
	void ipPortToInet6(const char* ip, PortType port, Ipv6Addr* addr);
};

#endif // NETS_INETUTIL_H
