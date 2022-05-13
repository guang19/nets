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
		explicit InetAddress(const char* ip, PortType port, bool ipv6 = false);

	public:
		static InetAddress createAnyInetAddress(PortType port, bool ipv6 = false);
		static InetAddress createLoopBackInetAddress(PortType port, bool ipv6 = false);

	public:
		inline sa_family_t ipFamily() const
		{
			return addr4_.sin_family;
		}

		::std::string ip() const;
		PortType port() const;
		::std::string toString() const;

	public:
		union
		{
			Ipv4Addr addr4_;
			Ipv6Addr addr6_;
		} ;
	};

	class Socket
	{
	};
} // namespace nets::net

#endif // NETS_SOCKET_H
