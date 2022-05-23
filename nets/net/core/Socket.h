//
// Created by n021949 on 2022/5/13.
//

#ifndef NETS_SOCKET_H
#define NETS_SOCKET_H

#include <netinet/in.h>

#include "nets/base/Copyable.h"
#include "nets/base/Noncopyable.h"

namespace nets::net
{
	using FdType = int32_t;
	using SockLenType = ::socklen_t;
	using SockAddr = struct sockaddr;
	using SockAddrFamily = ::sa_family_t;

	namespace socket
	{
		FdType createTcpSocket(SockAddrFamily family);
		FdType createTcpNonBlockSocket(SockAddrFamily family);
		FdType createUdpSocket(SockAddrFamily family);
		void closeSocket(FdType sockFd);

		void bind(FdType sockFd, const SockAddr* sockAddr);
		void listen(FdType sockFd);
		FdType accept(FdType sockFd, SockAddr* sockAddr);

		// usually, newer os all support dynamic sock buffer resizing
		// dont require manual set
		void setSockSendBuf(FdType sockFd, SockLenType sendBufLen);
		void setSockRecvBuf(FdType sockFd, SockLenType recvBufLen);
		void setSockAddrReuse(FdType sockFd, bool enable);
		void setSockPortReuse(FdType sockFd, bool enable);
		void setSockKeepAlive(FdType sockFd, bool enable);
		void setIpTcpNoDelay(FdType sockFd, bool enable);
		void setSockNonBlock(FdType sockFd, bool enable);
	} // namespace socket
} // namespace nets::net

#endif // NETS_SOCKET_H
