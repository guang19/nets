//
// Created by n021949 on 2022/5/13.
//

#ifndef NETS_NET_SOCKET_H
#define NETS_NET_SOCKET_H

#include "nets/net/core/InetSockAddress.h"

namespace nets::net
{
	namespace
	{
		using FdType = int32_t;
		using OptValType = int32_t;
		using SockLinger = struct linger;
	}

	namespace socket
	{
		constexpr FdType InvalidFd = -1;

		FdType createTcpSocket(SockAddrFamily family = AF_INET6);
		FdType createUdpSocket(SockAddrFamily family = AF_INET6);
		void closeFd(FdType fd);

		FdType createIdleFd();
		// EMFILE: The per-process limit of open file descriptors has been reached
		void dealwithEMFILE(FdType* idleFd, FdType sockFd);

		void bind(FdType sockFd, const SockAddr* sockAddr);
		void listen(FdType sockFd);
		FdType accept(FdType sockFd, SockAddr* sockAddr);

		::ssize_t read(FdType fd, void* buf, ::size_t n);
		::ssize_t write(FdType fd, const void* buf, ::size_t n);

		// usually, newer os all support dynamic sock buffer resizing, so dont require manual set wmem_default and rmem_default
		// SO_SNDBUF default value is 16384 bytes on linux which kernel version is 5.10.x
		void setSockSendBuf(FdType sockFd, OptValType sendBufLen = 16 * 1024);
		// SO_RCVBUF default value is 131072 bytes on linux which kernel version is 5.10.x
		void setSockRecvBuf(FdType sockFd, OptValType recvBufLen = 128 * 1024);
		void setSockAddrReuse(FdType sockFd, bool enable = true);
		void setSockPortReuse(FdType sockFd, bool enable = true);
		void setSockKeepAlive(FdType sockFd, bool enable = true);
		void setIpTcpNoDelay(FdType sockFd, bool enable = true);
		void setSockNonBlock(FdType sockFd, bool enable = true);
		// set socket linger
		void setSockLinger(FdType sockFd, const SockLinger& linger);
	} // namespace socket
} // namespace nets::net

#endif // NETS_NET_SOCKET_H
