//
// Created by n021949 on 2022/5/13.
//

#ifndef NETS_SOCKET_H
#define NETS_SOCKET_H

#include "nets/base/Copyable.h"
#include "nets/base/Noncopyable.h"
#include "nets/net/core/NetUtil.h"

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
		};
	};

	class Socket : base::Noncopyable
	{
	public:
		explicit Socket(FdType sockFd) : sockFd_(sockFd) {}
		~Socket() = default;

	public:
		// usually, newer os all support dynamic sock buffer resizing
		// dont require manual set
		void setSendBuf(SockLenType sendBufLen = 64 * 1024);
		void setRecvBuf(SockLenType recvBufLen = 64 * 1024);

		void setAddrReuse(bool enable = true);
		void setPortReuse(bool enable = true);
		void setKeepAlive(bool enable = true);
		void setTcpNoDelay(bool enable = true);

		void setNonBlock(bool enable = true);

	private:
		FdType sockFd_ {-1};
	};
} // namespace nets::net

#endif // NETS_SOCKET_H
