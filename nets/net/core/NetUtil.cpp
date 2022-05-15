//
// Created by n021949 on 2022/5/13.
//

#include "nets/net/core/NetUtil.h"

#include <arpa/inet.h>
#include <cstring>
#include <fcntl.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <unistd.h>

#include "nets/base/log/Logging.h"

namespace nets::net::util
{

	FdType createTcpSocket(SockAddrFamily family)
	{
		FdType sockFd = ::socket(family, SOCK_STREAM, IPPROTO_TCP);
		if (sockFd < 0)
		{
			LOGS_FATAL << "create tcp socket failed";
		}
		return sockFd;
	}

	FdType createNonBlockTcpSocket(SockAddrFamily family)
	{
		FdType sockFd = ::socket(family, SOCK_STREAM | O_NONBLOCK | FD_CLOEXEC, IPPROTO_TCP);
		if (sockFd < 0)
		{
			LOGS_FATAL << "create tcp socket failed";
		}
		return sockFd;
	}

	FdType createUdpSocket(SockAddrFamily family)
	{
		FdType sockFd = ::socket(family, SOCK_DGRAM, IPPROTO_UDP);
		if (sockFd < 0)
		{
			LOGS_ERROR << "create udp socket failed";
		}
		return sockFd;
	}

	void closeSocket(FdType sockFd)
	{
		if (sockFd > 0)
		{
			if (::close(sockFd) != 0)
			{
				LOGS_ERROR << "close socket failed";
			}
		}
	}

	PortType netToHost16(PortType netPort)
	{
		return be16toh(netPort);
	}

	PortType hostToNet16(PortType hostPort)
	{
		return htobe16(hostPort);
	}

	void createLoopBackInet4Addr(PortType port, Ipv4Addr* addr)
	{
		addr->sin_family = AF_INET;
		addr->sin_port = htobe16(port);
		addr->sin_addr.s_addr = INADDR_LOOPBACK;
	}

	void createLoopBackInet6Addr(PortType port, Ipv6Addr* addr)
	{
		addr->sin6_family = AF_INET6;
		addr->sin6_port = htobe16(port);
		addr->sin6_addr = in6addr_loopback;
	}

	void createAnyInet4Addr(PortType port, Ipv4Addr* addr)
	{
		addr->sin_family = AF_INET;
		addr->sin_port = htobe16(port);
		addr->sin_addr.s_addr = INADDR_ANY;
	}

	void createAnyInet6Addr(PortType port, Ipv6Addr* addr)
	{
		addr->sin6_family = AF_INET6;
		addr->sin6_port = htobe16(port);
		addr->sin6_addr = in6addr_any;
	}

	void ipPortToInet4Addr(const char* ip, PortType port, Ipv4Addr* addr)
	{
		addr->sin_family = AF_INET;
		addr->sin_port = htobe16(port);
		if (1 != ::inet_pton(AF_INET, ip, &(addr->sin_addr)))
		{
			LOGS_FATAL << "inet_pton AF_INET " << ip;
		}
	}

	void ipPortToInet6Addr(const char* ip, PortType port, Ipv6Addr* addr)
	{
		addr->sin6_family = AF_INET6;
		addr->sin6_port = htobe16(port);
		if (1 != ::inet_pton(AF_INET6, ip, &addr->sin6_addr))
		{
			LOGS_FATAL << "inet_pton AF_INET6 " << ip;
		}
	}

	void getIpFromSockAddr(const SockAddr* sockAddr, char* buffer, SockLenType len)
	{
		if (AF_INET == sockAddr->sa_family)
		{
			auto addr4 = reinterpret_cast<const Ipv4Addr*>(sockAddr);
			if (nullptr == ::inet_ntop(AF_INET, &addr4->sin_addr, buffer, len))
			{
				LOGS_FATAL << "inet_ntop AF_INET";
			}
		}
		else if (AF_INET6 == sockAddr->sa_family)
		{
			auto addr6 = reinterpret_cast<const Ipv6Addr*>(sockAddr);
			if (nullptr == ::inet_ntop(AF_INET6, &addr6->sin6_addr, buffer, len))
			{
				LOGS_FATAL << "inet_ntop AF_INET6";
			}
		}
	}

	void sockAddrToString(const SockAddr* sockAddr, char* buffer, ::size_t len)
	{
		if (AF_INET == sockAddr->sa_family)
		{
			getIpFromSockAddr(sockAddr, buffer, static_cast<SockLenType>(len));
			SockLenType ipLen = ::strlen(buffer);
			PortType port = be16toh(reinterpret_cast<const Ipv4Addr*>(sockAddr)->sin_port);
			::snprintf(buffer + ipLen, len - ipLen, ":%u", port);
		}
		else if (AF_INET6 == sockAddr->sa_family)
		{
			// ipv6 + port address like: [2a01:198:603:0:396e]:8080
			buffer[0] = '[';
			getIpFromSockAddr(sockAddr, buffer + 1, static_cast<SockLenType>(len - 1));
			SockLenType ipLen = ::strlen(buffer);
			PortType port = be16toh(reinterpret_cast<const Ipv6Addr*>(sockAddr)->sin6_port);
			::snprintf(buffer + ipLen, len - ipLen, "]:%u", port);
		}
	}

	void setSockSendBuf(FdType sockFd, SockLenType sendBufLen)
	{
		if (0 != ::setsockopt(sockFd, SOL_SOCKET, SO_SNDBUF, &sendBufLen, static_cast<SockLenType>(sendBufLen)))
		{
			LOGS_ERROR << "setsockopt SO_SNDBUF failed";
		}
	}

	void setSockRecvBuf(FdType sockFd, SockLenType recvBufLen)
	{
		if (0 != ::setsockopt(sockFd, SOL_SOCKET, SO_RCVBUF, &recvBufLen, static_cast<SockLenType>(recvBufLen)))
		{
			LOGS_ERROR << "setsockopt SO_RCVBUF failed";
		}
	}

	void setSockAddrReuse(FdType sockFd, bool enable)
	{
		SockLenType reuse = enable ? 1 : 0;
		if (0 != ::setsockopt(sockFd, SOL_SOCKET, SO_REUSEADDR, &reuse, static_cast<SockLenType>(sizeof(reuse))))
		{
			LOGS_ERROR << "setsockopt SO_REUSEADDR failed";
		}
	}

	void setSockPortReuse(PortType sockFd, bool enable)
	{
		SockLenType reuse = enable ? 1 : 0;
		if (0 != ::setsockopt(sockFd, SOL_SOCKET, SO_REUSEPORT, &reuse, static_cast<SockLenType>(sizeof(reuse))))
		{
			LOGS_ERROR << "setsockopt SO_REUSEPORT failed";
		}
	}

	void setSockKeepAlive(PortType sockFd, bool enable)
	{
		SockLenType keepAlive = enable ? 1 : 0;
		if (0 != ::setsockopt(sockFd, SOL_SOCKET, SO_KEEPALIVE, &keepAlive, static_cast<SockLenType>(sizeof(keepAlive))))
		{
			LOGS_ERROR << "setsockopt SO_KEEPALIVE failed";
		}
	}

	void setIpTcpNoDelay(FdType sockFd, bool enable)
	{
		SockLenType noDelay = enable ? 1 : 0;
		if (0 != ::setsockopt(sockFd, IPPROTO_TCP, TCP_NODELAY, &noDelay, static_cast<SockLenType>(sizeof(noDelay))))
		{
			LOGS_ERROR << "setsockopt TCP_NODELAY failed";
		}
	}

	void setSockNonBlock(FdType sockFd, bool enable)
	{
		int32_t flags = ::fcntl(sockFd, F_GETFL, 0);
		if (enable)
		{
			flags |= O_NONBLOCK;
		}
		else
		{
			flags &= ~O_NONBLOCK;
		}
		if (-1 == ::fcntl(sockFd, F_SETFL, flags))
		{
			LOGS_ERROR << "fcntl O_NONBLOCK failed";
		}
		flags = ::fcntl(sockFd, F_GETFD, 0);
		flags |= FD_CLOEXEC;
		if (-1 == ::fcntl(sockFd, F_SETFD, flags))
		{
			LOGS_ERROR << "fcntl FD_CLOEXEC failed";
		}
	}
} // namespace nets::net::util