//
// Created by n021949 on 2022/5/13.
//

#ifndef NETS_NETUTIL_H
#define NETS_NETUTIL_H

#include <cstdint>
#include <netinet/in.h>
#include <string>

namespace nets::net
{
	using FdType = int32_t;
	using SockLenType = ::socklen_t;
	using PortType = uint16_t;
	using SockAddr = struct sockaddr;
	using Ipv4Addr = struct sockaddr_in;
	using Ipv6Addr = struct sockaddr_in6;
	using SockAddrFamily = ::sa_family_t;

	namespace util
	{
		FdType createTcpSocket(SockAddrFamily family);
		FdType createTcpNonBlockSocket(SockAddrFamily family);
		FdType createUdpSocket(SockAddrFamily family);
		void closeSocket(FdType sockFd);

		PortType netToHost16(PortType netPort);
		PortType hostToNet16(PortType hostPort);

		void createLoopBackInet4Addr(PortType port, Ipv4Addr* addr);
		void createLoopBackInet6Addr(PortType port, Ipv6Addr* addr);

		void createAnyInet4Addr(PortType port, Ipv4Addr* addr);
		void createAnyInet6Addr(PortType port, Ipv6Addr* addr);

		void ipPortToInet4Addr(const char* ip, PortType port, Ipv4Addr* addr);
		void ipPortToInet6Addr(const char* ip, PortType port, Ipv6Addr* addr);

		void getIpFromSockAddr(const SockAddr* sockAddr, char* buffer, SockLenType len);

		void sockAddrToString(const SockAddr* sockAddr, char* buffer, ::size_t len);

		void setSockSendBuf(FdType sockFd, SockLenType sendBufLen);
		void setSockRecvBuf(FdType sockFd, SockLenType recvBufLen);
		void setSockAddrReuse(FdType sockFd, bool enable);
		void setSockPortReuse(PortType sockFd, bool enable);
		void setSockKeepAlive(PortType sockFd, bool enable);
		void setIpTcpNoDelay(FdType sockFd, bool enable);

		void setSockNonBlock(FdType sockFd, bool enable);
	} // namespace util
};    // namespace nets::net

#endif // NETS_NETUTIL_H
