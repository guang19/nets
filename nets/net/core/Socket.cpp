//
// Created by guang19 on 2022/5/13.
//

#include "nets/net/core/Socket.h"

#include <fcntl.h>
#include <netinet/tcp.h>
#include <sys/uio.h>
#include <unistd.h>

#include "nets/base/log/Logging.h"

namespace nets::net::socket
{
	FdType createTcpSocket(SockAddrFamily family)
	{
		FdType sockFd = ::socket(family, SOCK_STREAM, IPPROTO_TCP);
		if (sockFd < 0)
		{
			LOGS_FATAL << "socket::createTcpSocket failed";
		}
		return sockFd;
	}

	FdType createUdpSocket(SockAddrFamily family)
	{
		FdType sockFd = ::socket(family, SOCK_DGRAM, IPPROTO_UDP);
		if (sockFd < 0)
		{
			LOGS_FATAL << "socket::createUdpSocket failed";
		}
		return sockFd;
	}

	void closeFd(FdType fd)
	{
		if (fd >= 0)
		{
			if (0 != ::close(fd))
			{
				LOGS_ERROR << "socket::closeFd " << fd << " failed";
			}
		}
	}

	FdType createIdleFd()
	{
		FdType idleFd = ::open("/dev/null", O_RDONLY | O_CLOEXEC);
		if (idleFd < 0)
		{
			LOGS_ERROR << "socket::createIdleFd failed";
			return socket::InvalidFd;
		}
		return idleFd;
	}

	void dealwithEMFILE(FdType* idleFd, FdType sockFd)
	{
		closeFd(*idleFd);
		*idleFd = ::accept(sockFd, nullptr, nullptr);
		closeFd(*idleFd);
		*idleFd = createIdleFd();
	}

	void bind(FdType sockFd, const SockAddr* sockAddr)
	{
		auto len = static_cast<SockLenType>((sockAddr->sa_family == AF_INET ? sizeof(SockAddr4) : sizeof(SockAddr6)));
		if (0 != ::bind(sockFd, sockAddr, len))
		{
			LOGS_FATAL << "socket::bind failed";
		}
	}

	void connect(FdType sockFd, const SockAddr* sockAddr)
	{
		auto len = static_cast<SockLenType>((sockAddr->sa_family == AF_INET ? sizeof(SockAddr4) : sizeof(SockAddr6)));
		if (0 != connect(sockFd, sockAddr, len))
		{
			LOGS_FATAL << "socket::connect failed";
		}
	}

	void listen(FdType sockFd)
	{
		if (0 != ::listen(sockFd, SOMAXCONN))
		{
			LOGS_FATAL << "socket::listen failed";
		}
	}

	FdType acceptAddr4(FdType sockFd, SockAddr* sockAddr)
	{
		auto len = static_cast<SockLenType>(sizeof(SockAddr4));
		FdType connFd = InvalidFd;
		if ((connFd = ::accept4(sockFd, sockAddr, &len, SOCK_NONBLOCK | SOCK_CLOEXEC)) < 0)
		{
			LOGS_FATAL << "socket::accept failed";
		}
		return connFd;
	}

	FdType acceptAddr6(FdType sockFd, SockAddr* sockAddr)
	{
		auto len = static_cast<SockLenType>(sizeof(SockAddr6));
		FdType connFd = InvalidFd;
		if ((connFd = ::accept4(sockFd, sockAddr, &len, SOCK_NONBLOCK | SOCK_CLOEXEC)) < 0)
		{
			LOGS_FATAL << "socket::accept failed";
		}
		return connFd;
	}

	::ssize_t read(FdType fd, void* buf, ::size_t n)
	{
		return ::read(fd, buf, n);
	}

	::ssize_t readv(FdType fd, const IoVec* vec, int32_t iovcnt)
	{
		return ::readv(fd, vec, iovcnt);
	}

	::ssize_t write(FdType fd, const void* buf, ::size_t n)
	{
		return ::write(fd, buf, n);
	}

	void setSockSendBuf(FdType sockFd, OptValType sendBufLen)
	{
		if (0 != ::setsockopt(sockFd, SOL_SOCKET, SO_SNDBUF, &sendBufLen, static_cast<SockLenType>(sendBufLen)))
		{
			LOGS_ERROR << "socket::setSockSendBuf failed";
		}
	}

	void setSockRecvBuf(FdType sockFd, OptValType recvBufLen)
	{
		if (0 != ::setsockopt(sockFd, SOL_SOCKET, SO_RCVBUF, &recvBufLen, static_cast<SockLenType>(recvBufLen)))
		{
			LOGS_ERROR << "socket::setSockRecvBuf failed";
		}
	}

	void setSockAddrReuse(FdType sockFd, bool enable)
	{
		OptValType reuse = enable ? 1 : 0;
		if (0 != ::setsockopt(sockFd, SOL_SOCKET, SO_REUSEADDR, &reuse, static_cast<SockLenType>(sizeof(reuse))))
		{
			LOGS_FATAL << "socket::setSockAddrReuse failed";
		}
	}

	void setSockPortReuse(FdType sockFd, bool enable)
	{
		OptValType reuse = enable ? 1 : 0;
		if (0 != ::setsockopt(sockFd, SOL_SOCKET, SO_REUSEPORT, &reuse, static_cast<SockLenType>(sizeof(reuse))))
		{
			LOGS_FATAL << "socket::setSockPortReuse failed";
		}
	}

	void setSockKeepAlive(FdType sockFd, bool enable)
	{
		OptValType keepAlive = enable ? 1 : 0;
		if (0 != ::setsockopt(sockFd, SOL_SOCKET, SO_KEEPALIVE, &keepAlive, static_cast<SockLenType>(sizeof(keepAlive))))
		{
			LOGS_ERROR << "socket::setSockKeepAlive failed";
		}
	}

	void setIpTcpNoDelay(FdType sockFd, bool enable)
	{
		OptValType noDelay = enable ? 1 : 0;
		if (0 != ::setsockopt(sockFd, IPPROTO_TCP, TCP_NODELAY, &noDelay, static_cast<SockLenType>(sizeof(noDelay))))
		{
			LOGS_ERROR << "socket::setIpTcpNoDelay failed";
		}
	}

	void setSockLinger(FdType sockFd, const SockLinger& linger)
	{
		if (0 != ::setsockopt(sockFd, IPPROTO_TCP, SO_LINGER, &linger, static_cast<SockLenType>(sizeof(SockLinger))))
		{
			LOGS_ERROR << "socket::setSockLinger failed";
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
			LOGS_ERROR << "socket::setSockNonBlock failed";
		}
		flags = ::fcntl(sockFd, F_GETFD, 0);
		flags |= FD_CLOEXEC;
		if (-1 == ::fcntl(sockFd, F_SETFD, flags))
		{
			LOGS_ERROR << "socket::setSockNonBlock failed";
		}
	}
} // namespace nets::net::socket