//
// Created by guang19 on 2022/5/13.
//

#include "nets/net/core/Socket.h"

#include <fcntl.h>
#include <netinet/tcp.h>
#include <unistd.h>

#include "nets/base/log/Logging.h"
#include "nets/net/exception/SocketCreationException.h"
#include "nets/net/exception/SocketOperationException.h"

namespace nets::net::socket
{
    FdType createTcpSocket(SockAddrFamily family)
    {
        FdType sockFd = ::socket(family, SOCK_STREAM, IPPROTO_TCP);
        if (sockFd < 0)
        {
            THROW_FMT(SocketCreationException, "socket createTcpSocket failed");
        }
        setSockCloExec(sockFd);
        return sockFd;
    }

    FdType createUdpSocket(SockAddrFamily family)
    {
        FdType sockFd = ::socket(family, SOCK_DGRAM, IPPROTO_UDP);
        if (sockFd < 0)
        {
            THROW_FMT(SocketCreationException, "socket createUdpSocket failed");
        }
        setSockCloExec(sockFd);
        return sockFd;
    }

    void setSockCloExec(FdType sockFd)
    {
        int32_t flags = ::fcntl(sockFd, F_GETFD, 0);
        flags |= FD_CLOEXEC;
        if (-1 == ::fcntl(sockFd, F_SETFD, flags))
        {
            THROW_FMT(SocketOperationException, "socket set FD_CLOEXEC failed");
        }
    }

    void closeFd(FdType fd)
    {
        if (fd >= 0)
        {
            if (0 != ::close(fd))
            {
                LOGS_ERROR << "socket closeFd " << fd << " failed";
            }
        }
    }

    void shutdown(FdType sockFd, int32_t how)
    {
        if (0 != ::shutdown(sockFd, how))
        {
            LOGS_ERROR << "socket shutdown sockFd " << sockFd << " failed";
        }
    }

    FdType createIdleFd()
    {
        FdType idleFd = ::open("/dev/null", O_RDONLY | O_CLOEXEC);
        if (idleFd < 0)
        {
            LOGS_ERROR << "socket createIdleFd failed";
            return socket::InvalidFd;
        }
        return idleFd;
    }

    void dealwithEMFILE(FdType& idleFd, FdType sockFd)
    {
        closeFd(idleFd);
        FdType connFd = ::accept(sockFd, nullptr, nullptr);
        // close client connection, avoid listenFd always trigger
        closeFd(connFd);
        // recreate idle fd
        idleFd = createIdleFd();
    }

    void bind(FdType sockFd, const SockAddr* sockAddr)
    {
        auto len = static_cast<SockLenType>((sockAddr->sa_family == AF_INET ? sizeof(SockAddr4) : sizeof(SockAddr6)));
        if (0 != ::bind(sockFd, sockAddr, len))
        {
            THROW_FMT(SocketOperationException, "socket bind failed");
        }
    }

    void listen(FdType sockFd, int32_t backlog)
    {
        if (0 != ::listen(sockFd, backlog))
        {
            THROW_FMT(SocketOperationException, "socket listen failed");
        }
    }

    FdType accept(FdType sockFd, SockAddr6* sockAddr)
    {
        auto len = static_cast<SockLenType>(sizeof(SockAddr6));
        return ::accept4(sockFd, reinterpret_cast<SockAddr*>(sockAddr), &len, SOCK_NONBLOCK | SOCK_CLOEXEC);
    }

    int32_t connect(FdType sockFd, const SockAddr* sockAddr)
    {
        auto len = static_cast<SockLenType>((sockAddr->sa_family == AF_INET ? sizeof(SockAddr4) : sizeof(SockAddr6)));
        return ::connect(sockFd, sockAddr, len);
    }

    SSizeType read(FdType fd, void* buf, ::size_t n)
    {
        return ::read(fd, buf, n);
    }

    SSizeType write(FdType fd, const void* buf, ::size_t n)
    {
        return ::write(fd, buf, n);
    }

    void getLocalAddress(FdType fd, SockAddr6* sockAddr)
    {
        auto len = static_cast<SockLenType>(sizeof(SockAddr6));
        if (0 != ::getsockname(fd, reinterpret_cast<SockAddr*>(sockAddr), &len))
        {
            LOGS_ERROR << "socket getLocalAddress failed";
        }
    }

    void getPeerAddress(FdType fd, SockAddr6* sockAddr)
    {
        auto len = static_cast<SockLenType>(sizeof(SockAddr6));
        if (0 != ::getpeername(fd, reinterpret_cast<SockAddr*>(sockAddr), &len))
        {
            LOGS_ERROR << "socket getPeerAddress failed";
        }
    }

    OptValType getSockError(FdType sockFd)
    {
        OptValType optVal = 0;
        auto len = static_cast<SockLenType>(sizeof(optVal));
        if (0 != ::getsockopt(sockFd, SOL_SOCKET, SO_ERROR, &optVal, &len))
        {
            LOGS_ERROR << "socket getSockError failed";
            return errno;
        }
        else
        {
            return optVal;
        }
    }

    void setSockSendBuf(FdType sockFd, OptValType sendBufLen)
    {
        if (0 != ::setsockopt(sockFd, SOL_SOCKET, SO_SNDBUF, &sendBufLen, static_cast<SockLenType>(sendBufLen)))
        {
            LOGS_ERROR << "socket setSockSendBuf failed";
        }
    }

    void setSockRecvBuf(FdType sockFd, OptValType recvBufLen)
    {
        if (0 != ::setsockopt(sockFd, SOL_SOCKET, SO_RCVBUF, &recvBufLen, static_cast<SockLenType>(recvBufLen)))
        {
            LOGS_ERROR << "socket setSockRecvBuf failed";
        }
    }

    OptValType getTcpSockSendBuf()
    {
        FdType sockFd = socket::createTcpSocket(AF_INET);
        OptValType optVal = 0;
        auto len = static_cast<SockLenType>(sizeof(optVal));
        if (0 != ::getsockopt(sockFd, SOL_SOCKET, SO_SNDBUF, &optVal, &len))
        {
            LOGS_ERROR << "socket getTcpSockSendBuf failed";
        }
        socket::closeFd(sockFd);
        return optVal;
    }

    OptValType getTcpSockRecvBuf()
    {
        FdType sockFd = socket::createTcpSocket(AF_INET);
        OptValType optVal = 0;
        auto len = static_cast<SockLenType>(sizeof(optVal));
        if (0 != ::getsockopt(sockFd, SOL_SOCKET, SO_RCVBUF, &optVal, &len))
        {
            LOGS_ERROR << "socket getTcpSockRecvBuf failed";
        }
        socket::closeFd(sockFd);
        return optVal;
    }

    OptValType getUdpSockSendBuf()
    {
        FdType sockFd = socket::createUdpSocket(AF_INET);
        OptValType optVal = 0;
        auto len = static_cast<SockLenType>(sizeof(optVal));
        if (0 != ::getsockopt(sockFd, SOL_SOCKET, SO_SNDBUF, &optVal, &len))
        {
            LOGS_ERROR << "socket getUdpSockSendBuf failed";
        }
        socket::closeFd(sockFd);
        return optVal;
    }

    OptValType getUdpSockRecvBuf()
    {
        FdType sockFd = socket::createUdpSocket(AF_INET);
        OptValType optVal = 0;
        auto len = static_cast<SockLenType>(sizeof(optVal));
        if (0 != ::getsockopt(sockFd, SOL_SOCKET, SO_RCVBUF, &optVal, &len))
        {
            LOGS_ERROR << "socket getUdpSockRecvBuf failed";
        }
        socket::closeFd(sockFd);
        return optVal;
    }

    void setSockReuseAddr(FdType sockFd, bool enable)
    {
        OptValType reuse = enable ? 1 : 0;
        if (0 != ::setsockopt(sockFd, SOL_SOCKET, SO_REUSEADDR, &reuse, static_cast<SockLenType>(sizeof(reuse))))
        {
            LOGS_ERROR << "socket setSockAddrReuse failed";
        }
    }

    void setSockReusePort(FdType sockFd, bool enable)
    {
        OptValType reuse = enable ? 1 : 0;
        if (0 != ::setsockopt(sockFd, SOL_SOCKET, SO_REUSEPORT, &reuse, static_cast<SockLenType>(sizeof(reuse))))
        {
            LOGS_ERROR << "socket setSockPortReuse failed";
        }
    }

    void setSockKeepAlive(FdType sockFd, bool enable)
    {
        OptValType keepAlive = enable ? 1 : 0;
        if (0 != ::setsockopt(sockFd, SOL_SOCKET, SO_KEEPALIVE, &keepAlive, static_cast<SockLenType>(sizeof(keepAlive))))
        {
            LOGS_ERROR << "socket setSockKeepAlive failed";
        }
    }

    void setTcpNoDelay(FdType sockFd, bool enable)
    {
        OptValType noDelay = enable ? 1 : 0;
        if (0 != ::setsockopt(sockFd, IPPROTO_TCP, TCP_NODELAY, &noDelay, static_cast<SockLenType>(sizeof(noDelay))))
        {
            LOGS_ERROR << "socket setTcpNoDelay failed";
        }
    }

    void setSockLinger(FdType sockFd, const SockLinger& linger)
    {
        if (0 != ::setsockopt(sockFd, SOL_SOCKET, SO_LINGER, &linger, static_cast<SockLenType>(sizeof(SockLinger))))
        {
            LOGS_ERROR << "socket setSockLinger failed";
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
            LOGS_ERROR << "socket setSockNonBlock failed";
        }
    }
} // namespace nets::net::socket