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
        using SizeType = ::size_t;
        using SSizeType = ::ssize_t;
    } // namespace

    namespace socket
    {
        constexpr FdType InvalidFd = -1;

        FdType createTcpSocket(SockAddrFamily family = AF_INET6);
        FdType createUdpSocket(SockAddrFamily family = AF_INET6);
        void setSockCloExec(FdType sockFd);
        void closeFd(FdType fd);
        void shutdown(FdType sockFd, int32_t how);
        const char* shutdownHowToString(int32_t how);

        // EMFILE: The per-process limit of open file descriptors has been reached
        FdType createIdleFd();
        void dealwithEMFILE(FdType& idleFd, FdType sockFd);

        void bind(FdType sockFd, const SockAddr* sockAddr);
        void listen(FdType sockFd, int32_t backlog);
        FdType accept(FdType sockFd, SockAddr6* sockAddr);
        int32_t connect(FdType sockFd, const SockAddr* sockAddr);

        SSizeType read(FdType fd, void* buf, SizeType n);
        SSizeType readv(FdType fd, const IoVec* iov, int32_t iovcnt);
        SSizeType write(FdType fd, const void* buf, SizeType n);
        SSizeType writev(FdType fd, const IoVec* iov, int32_t iovcnt);


        void getLocalAddress(FdType fd, SockAddr6* sockAddr);
        void getPeerAddress(FdType fd, SockAddr6* sockAddr);

        OptValType getSockError(FdType sockFd);

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
        void setSockLinger(FdType sockFd, const SockLinger& linger);
    } // namespace socket

    namespace
    {
        const int32_t DefaultMaximumOfBackLog = SOMAXCONN;
        const int32_t DefaultSockLinger = 0;

        const int32_t DefaultTcpSockSendBufferSize = socket::getTcpSockSendBuf();
        const int32_t DefaultTcpSockRecvBufferSize = socket::getTcpSockRecvBuf();

        const int32_t DefaultUdpSockSendBufferSize = socket::getUdpSockSendBuf();
        const int32_t DefaultUdpSockRecvBufferSize = socket::getUdpSockRecvBuf();
    }
} // namespace nets::net

#endif // NETS_NET_SOCKET_H
