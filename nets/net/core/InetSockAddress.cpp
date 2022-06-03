//
// Created by guang19 on 2022/5/19.
//

#include "nets/net/core/InetSockAddress.h"

#include <arpa/inet.h>

#include "nets/base/CommonMacro.h"
#include "nets/base/log/Logging.h"

namespace nets::net
{
	InetSockAddress::InetSockAddress(const SockAddr4& addr4) : addr4_(addr4) {}

	InetSockAddress::InetSockAddress(const SockAddr6& addr6) : addr6_(addr6) {}

	InetSockAddress::InetSockAddress(const char* ip, PortType port, bool ipv6)
	{
		if (ipv6)
		{
			MEMZERO(&addr6_, sizeof(SockAddr6));
			addr6_.sin6_family = AF_INET6;
			addr6_.sin6_port = htobe16(port);
			if (1 != ::inet_pton(AF_INET6, ip, &addr6_.sin6_addr))
			{
				LOGS_FATAL << "inet_pton AF_INET6 " << ip;
			}
		}
		else
		{
			MEMZERO(&addr4_, sizeof(SockAddr4));
			addr4_.sin_family = AF_INET;
			addr4_.sin_port = htobe16(port);
			if (1 != ::inet_pton(AF_INET, ip, &(addr4_.sin_addr)))
			{
				LOGS_FATAL << "inet_pton AF_INET " << ip;
			}
		}
	}

	InetSockAddress::InetSockAddress(const InetSockAddress& addr)
	{
		if (addr.ipFamily() == AF_INET)
		{
			MEMZERO(&addr4_, sizeof(SockAddr4));
			addr4_ = addr.addr4_;
		}
		else
		{
			MEMZERO(&addr6_, sizeof(SockAddr6));
			addr6_ = addr.addr6_;
		}
	}

	InetSockAddress::InetSockAddress(InetSockAddress&& addr) noexcept
	{
		if (addr.ipFamily() == AF_INET)
		{
			MEMZERO(&addr4_, sizeof(SockAddr4));
			addr4_ = addr.addr4_;
			MEMZERO(&addr.addr4_, sizeof(SockAddr4));
		}
		else
		{
			MEMZERO(&addr6_, sizeof(SockAddr6));
			addr6_ = addr.addr6_;
			MEMZERO(&addr.addr6_, sizeof(SockAddr6));
		}
	}

	InetSockAddress& InetSockAddress::operator=(const InetSockAddress& addr)
	{
		if (this != &addr)
		{
			if (addr.ipFamily() == AF_INET)
			{
				MEMZERO(&addr4_, sizeof(SockAddr4));
				addr4_ = addr.addr4_;
			}
			else
			{
				MEMZERO(&addr6_, sizeof(SockAddr6));
				addr6_ = addr.addr6_;
			}
		}
		return *this;
	}

	InetSockAddress& InetSockAddress::operator=(InetSockAddress&& addr) noexcept
	{
		if (this != &addr)
		{
			if (addr.ipFamily() == AF_INET)
			{
				MEMZERO(&addr4_, sizeof(SockAddr4));
				addr4_ = addr.addr4_;
				MEMZERO(&addr.addr4_, sizeof(SockAddr4));
			}
			else
			{
				MEMZERO(&addr6_, sizeof(SockAddr6));
				addr6_ = addr.addr6_;
				MEMZERO(&addr.addr6_, sizeof(SockAddr6));
			}
		}
		return *this;
	}

	InetSockAddress InetSockAddress::createAnySockAddress(PortType port, bool ipv6)
	{
		if (ipv6)
		{
			SockAddr6 addr {};
			MEMZERO(&addr, sizeof(SockAddr6));
			addr.sin6_family = AF_INET6;
			addr.sin6_port = htobe16(port);
			addr.sin6_addr = in6addr_any;
			return InetSockAddress(addr);
		}
		else
		{
			SockAddr4 addr {};
			MEMZERO(&addr, sizeof(SockAddr4));
			addr.sin_family = AF_INET;
			addr.sin_port = htobe16(port);
			addr.sin_addr.s_addr = INADDR_ANY;
			return InetSockAddress(addr);
		}
	}

	InetSockAddress InetSockAddress::createLoopBackSockAddress(PortType port, bool ipv6)
	{
		if (ipv6)
		{
			SockAddr6 addr {};
			MEMZERO(&addr, sizeof(SockAddr6));
			addr.sin6_family = AF_INET6;
			addr.sin6_port = htobe16(port);
			addr.sin6_addr = in6addr_loopback;
			return InetSockAddress(addr);
		}
		else
		{
			SockAddr4 addr {};
			MEMZERO(&addr, sizeof(SockAddr4));
			addr.sin_family = AF_INET;
			addr.sin_port = htobe16(port);
			addr.sin_addr.s_addr = INADDR_LOOPBACK;
			return InetSockAddress(addr);
		}
	}
	::std::string InetSockAddress::ip() const
	{
		char buffer[64] = {0};
		auto len = static_cast<SockLenType>(sizeof(buffer));
		if (AF_INET == addr_.sa_family)
		{
			if (nullptr == ::inet_ntop(AF_INET, &addr4_.sin_addr, buffer, len))
			{
				LOGS_FATAL << "inet_ntop AF_INET";
			}
		}
		else if (AF_INET6 == addr_.sa_family)
		{
			if (nullptr == ::inet_ntop(AF_INET6, &addr6_.sin6_addr, buffer, len))
			{
				LOGS_FATAL << "inet_ntop AF_INET6";
			}
		}
		return buffer;
	}

	PortType InetSockAddress::port() const
	{
		return be16toh(addr4_.sin_port);
	}

	::std::string InetSockAddress::toString() const
	{
		char buffer[64] = {0};
		auto len = static_cast<SockLenType>(sizeof(buffer));
		if (AF_INET == addr_.sa_family)
		{
			if (nullptr == ::inet_ntop(AF_INET, &addr4_.sin_addr, buffer, len))
			{
				LOGS_FATAL << "inet_ntop AF_INET";
			}
			SockLenType ipLen = ::strlen(buffer);
			PortType port = be16toh(addr4_.sin_port);
			::snprintf(buffer + ipLen, len - ipLen, ":%u", port);
		}
		else if (AF_INET6 == addr_.sa_family)
		{
			// ipv6 + port address like: [2a01:198:603:0:396e]:8080
			buffer[0] = '[';
			if (nullptr == ::inet_ntop(AF_INET6, &addr6_.sin6_addr, buffer + 1, len))
			{
				LOGS_FATAL << "inet_ntop AF_INET6";
			}
			SockLenType ipLen = ::strlen(buffer);
			PortType port = be16toh(addr6_.sin6_port);
			::snprintf(buffer + ipLen, len - ipLen, "]:%u", port);
		}
		return buffer;
	}
} // namespace nets::net