//
// Created by guang19
//

#ifndef NETS_NET_SOCKET_H
#define NETS_NET_SOCKET_H

#include "nets/net/core/InetSockAddress.h"

namespace nets::net
{
    namespace
    {
        using FdType = ::int32_t;
        using OptValType = ::int32_t;
        using SockLinger = struct linger;
        using IoVec = struct iovec;
        using InAddr = struct in_addr;
        using IpMreqn = struct ip_mreqn;
        using Ipv6Mreq = struct ipv6_mreq;
        using SizeType = ::size_t;
        using SSizeType = ::ssize_t;
    } // namespace

    namespace socket
    {
        constexpr FdType gInvalidFd = -1;

        FdType createTcpSocket(SockAddrFamily family = AF_INET);
        FdType createUdpSocket(SockAddrFamily family = AF_INET);
        void setSockCloExec(FdType sockFd);
        void closeFd(FdType fd);
        void shutdown(FdType sockFd, ::int32_t how);
        const char* shutdownHowToString(::int32_t how);

        // EMFILE: The per-process limit of open file descriptors has been reached
        FdType createIdleFd();
        void dealwithEMFILE(FdType& idleFd, FdType sockFd);

        void bind(FdType sockFd, const InetSockAddress& localAddr);
        void listen(FdType sockFd, ::int32_t backlog);
        FdType accept(FdType sockFd, InetSockAddress& localAddr);
        ::int32_t connect(FdType sockFd, const InetSockAddress& peerAddr);

        SSizeType read(FdType fd, void* buf, SizeType n);
        SSizeType readv(FdType fd, const IoVec* iov, ::int32_t iovcnt);
        SSizeType write(FdType fd, const void* buf, SizeType n);
        SSizeType writev(FdType fd, const IoVec* iov, ::int32_t iovcnt);

        SSizeType sendTo(FdType fd, const void* buf, SizeType n, const InetSockAddress& destAddr);
        SSizeType recvFrom(FdType fd, void* buf, SizeType n, InetSockAddress& srcAddr);

        void getLocalAddress(FdType fd, InetSockAddress& sockAddr);
        void getPeerAddress(FdType fd, InetSockAddress& sockAddr);

        OptValType getSockError(FdType sockFd);

        void setSockReuseAddr(FdType sockFd, bool enable);
        void setSockReusePort(FdType sockFd, bool enable);
        void setSockKeepAlive(FdType sockFd, bool enable);
        void setSockKeepIdle(FdType sockFd, OptValType idleTime);
        void setSockKeepCnt(FdType sockFd, OptValType cnt);
        void setSockKeepIntvl(FdType sockFd, OptValType interval);
        void setTcpNoDelay(FdType sockFd, bool enable);
        void setSockNonBlock(FdType sockFd, bool enable);
        void setSockLinger(FdType sockFd, const SockLinger& linger);

        void setSockBroadCast(FdType sockFd, bool enable);

        // set multicast interface
        void setIpMultiCastIf(FdType sockFd, const ::std::string& ifAddr /** interface address**/);
        void setIpv6MultiCastIf(FdType sockFd, const ::std::string& ifName);

        // set multicast TTL
        void setIpMultiCastTTL(FdType sockFd, uint8_t ttl);
        void setIpv6MultiCastHops(FdType sockFd, uint8_t hops);

        // set multicast send loop enable
        void setIpMultiCastLoop(FdType sockFd, bool enable);
        void setIpv6MultiCastLoop(FdType sockFd, bool enable);

        // join group
        void addIpMemberShipByLocalAddr(FdType sockFd, const ::std::string& multicastAddr, const ::std::string& ifAddr);
        void addIpMemberShipByIfIndex(FdType sockFd, const ::std::string& multicastAddr, const ::std::string& inf);
        void addIpv6MemberShip(FdType sockFd, const ::std::string& multicastAddr, const ::std::string& inf);

        // leave group
        void dropIpMemberShipByLocalAddr(FdType sockFd, const ::std::string& multicastAddr, const ::std::string& localAddr);
        void dropIpMemberShipByIfIndex(FdType sockFd, const ::std::string& multicastAddr, const ::std::string& inf);
        void dropIpv6MemberShip(FdType sockFd, const ::std::string& multicastAddr, const ::std::string& inf);

        // usually, newer os all support dynamic sock buffer resizing, so dont require manual set wmem_default and
        // rmem_default TCP SO_SNDBUF default value is 16384 bytes on linux which kernel version is 5.10.x note:UDP has no
        // send buffer,for UDP,SO_SNDBUF just sets the maximum size of a single datagram sent UDP SO_SNDBUF default value is
        // 212992 bytes on linux which kernel version is 5.10.x
        void setSockSendBuf(FdType sockFd, OptValType sendBufLen);
        // TCP SO_RCVBUF default value is 131072 bytes on linux which kernel version is 5.10.x
        // UDP SO_RCVBUF default value is 212992 bytes on linux which kernel version is 5.10.x
        void setSockRecvBuf(FdType sockFd, OptValType recvBufLen);
        OptValType getTcpSockSendBuf();
        OptValType getTcpSockRecvBuf();
        OptValType getUdpSockSendBuf();
        OptValType getUdpSockRecvBuf();
    } // namespace socket

    namespace
    {
        const ::int32_t DefaultMaximumOfBackLog = SOMAXCONN;
        const ::int32_t DefaultSockLinger = 0;

        const ::int32_t DefaultTcpSockSendBufferSize = socket::getTcpSockSendBuf();
        const ::int32_t DefaultTcpSockRecvBufferSize = socket::getTcpSockRecvBuf();

        const ::int32_t DefaultUdpSockSendBufferSize = socket::getUdpSockSendBuf();
        const ::int32_t DefaultUdpSockRecvBufferSize = socket::getUdpSockRecvBuf();
    } // namespace
} // namespace nets::net

#endif // NETS_NET_SOCKET_H