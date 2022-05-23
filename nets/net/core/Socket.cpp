//
// Created by n021949 on 2022/5/13.
//

#include "nets/net/core/Socket.h"

#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <unistd.h>

#include "nets/base/log/Logging.h"

namespace nets::net::socket
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
		if (sockFd >= 0)
		{
			if (0 != ::close(sockFd))
			{
				LOGS_ERROR << "close socket failed";
			}
		}
	}

	void bind(FdType sockFd, const SockAddr* sockAddr)
	{
		auto len = static_cast<SockLenType>(sizeof(SockAddr));
		if (0 != ::bind(sockFd, sockAddr, len))
		{
			LOGS_FATAL << "bind socket failed";
		}
	}

	void listen(FdType sockFd)
	{
		if (0 != ::listen(sockFd, SOMAXCONN))
		{
			LOGS_FATAL << "listen socket failed";
		}
	}

	FdType accept(FdType sockFd, SockAddr* sockAddr)
	{
		auto len = static_cast<SockLenType>(sizeof(SockAddr));
		FdType connFd = -1;
		if ((connFd = ::accept4(sockFd, sockAddr, &len, SOCK_NONBLOCK | SOCK_CLOEXEC)) < 0)
		{
			LOGS_FATAL << "accept4 socket failed";
		}
		return connFd;
	}

	void setSockSendBuf(FdType sockFd, OptValType sendBufLen)
	{
		if (0 != ::setsockopt(sockFd, SOL_SOCKET, SO_SNDBUF, &sendBufLen, static_cast<SockLenType>(sendBufLen)))
		{
			LOGS_ERROR << "setsockopt SO_SNDBUF failed";
		}
	}

	void setSockRecvBuf(FdType sockFd, OptValType recvBufLen)
	{
		if (0 != ::setsockopt(sockFd, SOL_SOCKET, SO_RCVBUF, &recvBufLen, static_cast<SockLenType>(recvBufLen)))
		{
			LOGS_ERROR << "setsockopt SO_RCVBUF failed";
		}
	}

	void setSockAddrReuse(FdType sockFd, bool enable)
	{
		OptValType reuse = enable ? 1 : 0;
		if (0 != ::setsockopt(sockFd, SOL_SOCKET, SO_REUSEADDR, &reuse, static_cast<SockLenType>(sizeof(reuse))))
		{
			LOGS_ERROR << "setsockopt SO_REUSEADDR failed";
		}
	}

	void setSockPortReuse(FdType sockFd, bool enable)
	{
		OptValType reuse = enable ? 1 : 0;
		if (0 != ::setsockopt(sockFd, SOL_SOCKET, SO_REUSEPORT, &reuse, static_cast<SockLenType>(sizeof(reuse))))
		{
			LOGS_ERROR << "setsockopt SO_REUSEPORT failed";
		}
	}

	void setSockKeepAlive(FdType sockFd, bool enable)
	{
		OptValType keepAlive = enable ? 1 : 0;
		if (0 != ::setsockopt(sockFd, SOL_SOCKET, SO_KEEPALIVE, &keepAlive, static_cast<SockLenType>(sizeof(keepAlive))))
		{
			LOGS_ERROR << "setsockopt SO_KEEPALIVE failed";
		}
	}

	void setIpTcpNoDelay(FdType sockFd, bool enable)
	{
		OptValType noDelay = enable ? 1 : 0;
		if (0 != ::setsockopt(sockFd, IPPROTO_TCP, TCP_NODELAY, &noDelay, static_cast<SockLenType>(sizeof(noDelay))))
		{
			LOGS_ERROR << "setsockopt TCP_NODELAY failed";
		}
	}

	void setSockLinger(FdType sockFd, const SockLinger& linger)
	{
		if (0 != ::setsockopt(sockFd, IPPROTO_TCP, SO_LINGER, &linger, static_cast<SockLenType>(sizeof(SockLinger))))
		{
			LOGS_ERROR << "setsockopt SO_LINGER failed";
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
} // namespace nets::net::socket