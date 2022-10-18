// MIT License
//
// Copyright (c) 2022 guang19
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

// @brief The encapsulation of socket operations

#ifndef NETS_SOCKET_H
#define NETS_SOCKET_H

#include "nets/net/core/InetSockAddress.h"

namespace nets
{
    namespace
    {
        using FdType = Int32Type;
        using OptValType = Int32Type;
        using SockLinger = struct linger;
        using IoVec = struct iovec;
        using InAddr = struct in_addr;
        using IpMreqn = struct ip_mreqn;
        using Ipv6Mreq = struct ipv6_mreq;
        using SSizeType = ::ssize_t;
    } // namespace

    namespace socket
    {
        constexpr FdType kInvalidFd = -1;

        FdType createTcpSocket(SockAddrFamily family = AF_INET);
        FdType createUdpSocket(SockAddrFamily family = AF_INET);
        void setSockCloExec(FdType sockFd);
        void closeFd(FdType fd);
        void shutdown(FdType sockFd, Int32Type how);
        const char* shutdownHowToString(Int32Type how);

        // EMFILE: The per-process limit of open file descriptors has been reached
        FdType createIdleFd();
        void dealwithEMFILE(FdType& idleFd, FdType sockFd);

        void bind(FdType sockFd, const InetSockAddress& localAddr);
        void listen(FdType sockFd, Int32Type backlog);
        FdType accept(FdType sockFd, InetSockAddress& localAddr);
        Int32Type connect(FdType sockFd, const InetSockAddress& peerAddr);

        SSizeType read(FdType fd, void* buf, SizeType n);
        SSizeType readv(FdType fd, const IoVec* iov, Int32Type iovcnt);
        SSizeType write(FdType fd, const void* buf, SizeType n);
        SSizeType writev(FdType fd, const IoVec* iov, Int32Type iovcnt);

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

        void setSockBroadcast(FdType sockFd, bool enable);

        // set multicast interface
        void setIpMulticastIf(FdType sockFd, const StringType& ifAddr /** interface address**/);
        void setIpv6MulticastIf(FdType sockFd, const StringType& ifName);

        // set multicast TTL
        void setIpMulticastTTL(FdType sockFd, Uint8Type ttl);
        void setIpv6MulticastHops(FdType sockFd, Uint8Type hops);

        // set multicast send loop enable
        void setIpMulticastLoop(FdType sockFd, bool enable);
        void setIpv6MulticastLoop(FdType sockFd, bool enable);

        // join group
        bool addIpMemberShipByIfAddr(FdType sockFd, const StringType& multicastAddr, const StringType& ifAddr);
        bool addIpMemberShipByIfIndex(FdType sockFd, const StringType& multicastAddr, const StringType& inf);
        bool addIpv6MemberShip(FdType sockFd, const StringType& multicastAddr, const StringType& inf);

        // leave group
        bool dropIpMemberShipByIfAddr(FdType sockFd, const StringType& multicastAddr, const StringType& ifAddr);
        bool dropIpMemberShipByIfIndex(FdType sockFd, const StringType& multicastAddr, const StringType& inf);
        bool dropIpv6MemberShip(FdType sockFd, const StringType& multicastAddr, const StringType& inf);

        // usually, newer os all support dynamic sock buffer resizing, so dont require manual set wmem_default and
        // rmem_default TCP SO_SNDBUF default value is 16384 bytes on linux which kernel version is 5.10.x note:UDP has no
        // send buffer,for UDP,SO_SNDBUF just sets the maximum size of a single datagram sent UDP SO_SNDBUF default value is
        // 212992 bytes on linux which kernel version is 5.10.x
        void setSockSendBuf(FdType sockFd, OptValType sendBufLen);
        // TCP SO_RCVBUF default value is 131072 bytes on linux which kernel version is 5.10.x
        // UDP SO_RCVBUF default value is 212992 bytes on linux which kernel version is 5.10.x
        void setSockRecvBuf(FdType sockFd, OptValType recvBufLen);
        extern OptValType getTcpSockSendBuf();
        extern OptValType getTcpSockRecvBuf();
        extern OptValType getUdpSockSendBuf();
        extern OptValType getUdpSockRecvBuf();
    } // namespace socket

    namespace
    {
        constexpr Int32Type kDefaultMaximumOfBackLog = SOMAXCONN;

        static const Int32Type kDefaultTcpSockSendBufferSize = socket::getTcpSockSendBuf();
        static const Int32Type kDefaultTcpSockRecvBufferSize = socket::getTcpSockRecvBuf();

        static const Int32Type kDefaultUdpSockSendBufferSize = socket::getUdpSockSendBuf();
        static const Int32Type kDefaultUdpSockRecvBufferSize = socket::getUdpSockRecvBuf();
    } // namespace
} // namespace nets

#endif // NETS_SOCKET_H