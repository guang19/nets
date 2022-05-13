//
// Created by n021949 on 2022/5/13.
//

#include "nets/net/core/InetUtil.h"

#include <arpa/inet.h>

namespace nets::net
{
	void strToInet4(const ::std::string& str, Ipv4Addr* addr)
	{
		::std::string::size_type i = str.find_last_of(':');
		ipPortToInet4(str.substr(0, i).c_str(), static_cast<PortType>(::std::stoi(str.substr(i + 1))), addr);
	}

	void strToInet6(const ::std::string& str, Ipv6Addr* addr)
	{
		::std::string::size_type i = str.find_last_of(':');
		ipPortToInet6(str.substr(0, i).c_str(), static_cast<PortType>(::atoi(str.substr(i + 1).c_str())), addr);
	}

	void ipPortToInet4(const char* ip, PortType port, Ipv4Addr* addr)
	{
		addr->sin_family = AF_INET;
		addr->sin_port = ::htobe16(port);
		if (::inet_pton(AF_INET, ip, &(addr->sin_addr)) <= 0)
		{
			::fprintf(::stderr, "Error:inet_pton\n");
			exit(1);
		}
	}

	void ipPortToInet6(const char* ip, PortType port, Ipv6Addr* addr)
	{
		addr->sin6_family = AF_INET6;
		addr->sin6_port = ::htobe16(port);
		if (::inet_pton(AF_INET6, ip, &addr->sin6_addr))
		{
			::fprintf(::stderr, "Error:inet_pton\n");
			exit(1);
		}
	}
} // namespace nets::net