//
// Created by guang19 on 2022/5/19.
//

#ifndef NETS_INETADDRESS_H
#define NETS_INETADDRESS_H

#include "nets/net/core/NetUtil.h"
#include "nets/base/Copyable.h"

namespace nets::net
{
	class InetAddress : public base::Copyable
	{
	public:
		explicit InetAddress(const SockAddr4& addr4);
		explicit InetAddress(const SockAddr6& addr6);
		explicit InetAddress(const char* ip, PortType port, bool ipv4 = true);
		~InetAddress() = default;

		InetAddress(const InetAddress& addr);
		InetAddress(InetAddress&& addr) noexcept;
		InetAddress& operator=(const InetAddress& addr);
		InetAddress& operator=(InetAddress&& addr) noexcept;

	public:
		static InetAddress createAnyInetAddress(PortType port, bool ipv4 = true);
		static InetAddress createLoopBackInetAddress(PortType port, bool ipv4 = true);

	public:
		inline sa_family_t ipFamily() const
		{
			return addr4_.sin_family;
		}

		inline const SockAddr* cSockAddr() const
		{
			return &addr_;
		}

		inline SockAddr* sockAddr()
		{
			return &addr_;
		}

		::std::string ip() const;
		PortType port() const;
		::std::string toString() const;

	private:
		union
		{
			SockAddr addr_;
			SockAddr4 addr4_;
			SockAddr6 addr6_;
		};
	};
} // namespace nets::net

#endif // NETS_INETADDRESS_H
