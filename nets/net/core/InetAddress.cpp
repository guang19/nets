//
// Created by guang19 on 2022/5/19.
//

#include "nets/net/core/InetAddress.h"

#include "nets/base/CommonMacro.h"

namespace nets::net
{
	InetAddress::InetAddress(const SockAddr4& addr4) : addr4_(addr4) {}

	InetAddress::InetAddress(const SockAddr6& addr6) : addr6_(addr6) {}

	InetAddress::InetAddress(const char* ip, PortType port, bool ipv4)
	{
		if (ipv4)
		{
			MEMZERO(&addr4_, sizeof(SockAddr4));
			util::ipPortToInet4Addr(ip, port, &addr4_);
		}
		else
		{
			MEMZERO(&addr6_, sizeof(SockAddr6));
			util::ipPortToInet6Addr(ip, port, &addr6_);
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
			util::createAnyInet4Addr(port, &addr);
			return InetAddress(addr);
		}
		else
		{
			SockAddr6 addr {};
			MEMZERO(&addr, sizeof(SockAddr6));
			util::createAnyInet6Addr(port, &addr);
			return InetAddress(addr);
		}
	}

	InetAddress InetAddress::createLoopBackInetAddress(PortType port, bool ipv4)
	{
		if (ipv4)
		{
			SockAddr4 addr {};
			MEMZERO(&addr, sizeof(SockAddr4));
			util::createLoopBackInet4Addr(port, &addr);
			return InetAddress(addr);
		}
		else
		{
			SockAddr6 addr {};
			MEMZERO(&addr, sizeof(SockAddr6));
			util::createLoopBackInet6Addr(port, &addr);
			return InetAddress(addr);
		}
	}
	::std::string InetAddress::ip() const
	{
		char buffer[64] = {0};
		util::getIpFromSockAddr(&addr_, buffer, static_cast<SockLenType>(sizeof(buffer)));
		return buffer;
	}

	PortType InetAddress::port() const
	{
		return util::netToHost16(addr4_.sin_port);
	}

	::std::string InetAddress::toString() const
	{
		char buffer[64] = {0};
		util::sockAddrToString(&addr_, buffer, sizeof(buffer));
		return buffer;
	}
} // namespace nets::net