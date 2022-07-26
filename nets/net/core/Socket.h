//
// Created by guang19 on 2022/5/13.
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
        using IoVec = struct iovec;
    } // namespace

    namespace socket
    {
        constexpr FdType InvalidFd = -1;

        FdType createTcpSocket(SockAddrFamily family = AF_INET6);
        FdType createUdpSocket(SockAddrFamily family = AF_INET6);
        void setSockCloExec(FdType sockFd);
        void closeFd(FdType fd);

        FdType createIdleFd();
        // EMFILE: The per-process limit of open file descriptors has been reached
        void dealwithEMFILE(FdType* idleFd, FdType sockFd);

        void bind(FdType sockFd, const SockAddr* sockAddr);
        void connect(FdType sockFd, const SockAddr* sockAddr);

        void listen(FdType sockFd, int32_t backlog);
        FdType accept(FdType sockFd, SockAddr* sockAddr, FdType* idleFd);

        ::ssize_t read(FdType fd, void* buf, ::size_t n);
        ::ssize_t write(FdType fd, const void* buf, ::size_t n);

        ::ssize_t readv(FdType fd, const IoVec* vec, int32_t iovcnt);

        // usually, newer os all support dynamic sock buffer resizing, so dont require manual set wmem_default and rmem_default
        // TCP SO_SNDBUF default value is 16384 bytes on linux which kernel version is 5.10.x
        // UDP SO_SNDBUF default value is 212992 bytes on linux which kernel version is 5.10.x
        void setSockSendBuf(FdType sockFd, OptValType sendBufLen);
        // TCP SO_RCVBUF default value is 131072 bytes on linux which kernel version is 5.10.x
        // UDP SO_RCVBUF default value is 212992 bytes on linux which kernel version is 5.10.x
        void setSockRecvBuf(FdType sockFd, OptValType recvBufLen);
        OptValType getTcpSockSendBuf();
        OptValType getTcpSockRecvBuf();
        OptValType getUdpSockSendBuf();
        OptValType getUdpSockRecvBuf();

        void setSockReuseAddr(FdType sockFd, bool enable = true);
        void setSockReusePort(FdType sockFd, bool enable = true);
        void setSockKeepAlive(FdType sockFd, bool enable = true);
        void setTcpNoDelay(FdType sockFd, bool enable = true);
        void setSockNonBlock(FdType sockFd, bool enable = true);
        // set socket linger
        void setSockLinger(FdType sockFd, const SockLinger& linger);
    } // namespace socket

    namespace
    {
        const int32_t DefaultMaximumOfBackLog = SOMAXCONN;
        const int32_t DefaultSockLinger = 0;

        const int32_t DefaultTcpSockSendBufLen = socket::getTcpSockSendBuf();
        const int32_t DefaultTcpSockRecvBufLen = socket::getTcpSockRecvBuf();

        const int32_t DefaultUdpSockSendBufLen = socket::getUdpSockSendBuf();
        const int32_t DefaultUdpSockRecvBufLen = socket::getUdpSockRecvBuf();
    }
} // namespace nets::net

#endif // NETS_NET_SOCKET_H
