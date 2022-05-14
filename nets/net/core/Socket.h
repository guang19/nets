//
// Created by n021949 on 2022/5/13.
//

#ifndef NETS_SOCKET_H
#define NETS_SOCKET_H

#include "nets/net/core/InetUtil.h"

namespace nets::net
{
	class InetAddress
	{
	public:
		explicit InetAddress(const Ipv4Addr& addr4);
		explicit InetAddress(const Ipv6Addr& addr6);
		explicit InetAddress(const char* ip, PortType port, bool ipv4 = true);

	public:
		static InetAddress createAnyInetAddress(PortType port, bool ipv4 = true);
		static InetAddress createLoopBackInetAddress(PortType port, bool ipv4 = true);

	public:
		inline sa_family_t ipFamily() const
		{
			return addr4_.sin_family;
		}

		::std::string ip() const;
		PortType port() const;
		::std::string toString() const;

	private:
		union
		{
			SockAddr addr_;
			Ipv4Addr addr4_;
			Ipv6Addr addr6_;
		} ;
	};

	class Socket
	{
	};
} // namespace nets::net

#endif // NETS_SOCKET_H
