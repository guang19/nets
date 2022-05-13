//
// Created by n021949 on 2022/5/13.
//

#ifndef NETS_SOCKET_H
#define NETS_SOCKET_H

#include <string>

#include "nets/net/core/InetUtil.h"

namespace nets::net
{
	struct InetAddress
	{
	public:
		explicit InetAddress(const Ipv4Addr& addr4);
		explicit InetAddress(const Ipv6Addr& addr6);
		explicit InetAddress(const ::std::string& addr, bool ipv6 = false);
		explicit InetAddress(const char* ip, PortType port, bool ipv6 = false);

	public:


	public:
		union
		{
			Ipv4Addr addr4_;
			Ipv6Addr addr6_;
		};
	};

	class Socket
	{
	};
} // namespace nets::net

#endif // NETS_SOCKET_H
