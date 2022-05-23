//
// Created by guang19 on 2022/5/19.
//

#include "nets/net/core/InetAddress.h"

#include <arpa/inet.h>

#include "nets/base/CommonMacro.h"
#include "nets/base/log/Logging.h"

namespace nets::net
{
	InetAddress::InetAddress(const SockAddr4& addr4) : addr4_(addr4) {}

	InetAddress::InetAddress(const SockAddr6& addr6) : addr6_(addr6) {}

	InetAddress::InetAddress(const char* ip, PortType port, bool ipv4)
	{
		if (ipv4)
		{
			MEMZERO(&addr4_, sizeof(SockAddr4));
			addr4_.sin_family = AF_INET;
			addr4_.sin_port = htobe16(port);
			if (1 != ::inet_pton(AF_INET, ip, &(addr4_.sin_addr)))
			{
				LOGS_FATAL << "inet_pton AF_INET " << ip;
			}
		}
		else
		{
			MEMZERO(&addr6_, sizeof(SockAddr6));
			addr6_.sin6_family = AF_INET6;
			addr6_.sin6_port = htobe16(port);
			if (1 != ::inet_pton(AF_INET6, ip, &addr6_.sin6_addr))
			{
				LOGS_FATAL << "inet_pton AF_INET6 " << ip;
			}
		}
	}

	InetAddress::InetAddress(const InetAddress& addr)
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

	InetAddress::InetAddress(InetAddress&& addr) noexcept
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

	InetAddress&  InetAddress::operator=(const InetAddress& addr)
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

	InetAddress& InetAddress::operator=(InetAddress&& addr) noexcept
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

	InetAddress InetAddress::createAnyInetAddress(PortType port, bool ipv4)
	{
		if (ipv4)
		{
			SockAddr4 addr {};
			MEMZERO(&addr, sizeof(SockAddr4));
			addr.sin_family = AF_INET;
			addr.sin_port = htobe16(port);
			addr.sin_addr.s_addr = INADDR_ANY;
			return InetAddress(addr);
		}
		else
		{
			SockAddr6 addr {};
			MEMZERO(&addr, sizeof(SockAddr6));
			addr.sin6_family = AF_INET6;
			addr.sin6_port = htobe16(port);
			addr.sin6_addr = in6addr_any;
			return InetAddress(addr);
		}
	}

	InetAddress InetAddress::createLoopBackInetAddress(PortType port, bool ipv4)
	{
		if (ipv4)
		{
			SockAddr4 addr {};
			MEMZERO(&addr, sizeof(SockAddr4));
			addr.sin_family = AF_INET;
			addr.sin_port = htobe16(port);
			addr.sin_addr.s_addr = INADDR_LOOPBACK;
			return InetAddress(addr);
		}
		else
		{
			SockAddr6 addr {};
			MEMZERO(&addr, sizeof(SockAddr6));
			addr.sin6_family = AF_INET6;
			addr.sin6_port = htobe16(port);
			addr.sin6_addr = in6addr_loopback;
			return InetAddress(addr);
		}
	}
	::std::string InetAddress::ip() const
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

	PortType InetAddress::port() const
	{
		return be16toh(addr4_.sin_port);
	}

	::std::string InetAddress::toString() const
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
			if (nullptr == ::inet_ntop(AF_INET6, &addr6_.sin6_addr, buffer, len))
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