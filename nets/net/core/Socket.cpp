//
// Created by guang19
//

#include "nets/net/core/Socket.h"

#include <fcntl.h>
#include <netinet/tcp.h>
#include <sys/uio.h>
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
        ::int32_t flags = ::fcntl(sockFd, F_GETFD, 0);
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

    void shutdown(FdType sockFd, ::int32_t how)
    {
        if (0 != ::shutdown(sockFd, how))
        {
            LOGS_ERROR << "socket shutdown " << shutdownHowToString(how) << " failed";
        }
    }

    const char* shutdownHowToString(::int32_t how)
    {
        switch (how)
        {
            case SHUT_WR:
                return "write";
            case SHUT_RD:
                return "read";
            case SHUT_RDWR:
                return "both";
            default:
                return "unknown shutdown operation";
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

    void bind(FdType sockFd, const InetSockAddress& localAddr)
    {
        auto length = static_cast<SockLenType>((localAddr.ipFamily() == AF_INET ? sizeof(SockAddr4) : sizeof(SockAddr6)));
        if (0 != ::bind(sockFd, localAddr.sockAddr(), length))
        {
            THROW_FMT(SocketOperationException, "socket bind failed");
        }
    }

    void listen(FdType sockFd, ::int32_t backlog)
    {
        if (0 != ::listen(sockFd, backlog))
        {
            THROW_FMT(SocketOperationException, "socket listen failed");
        }
    }

    FdType accept(FdType sockFd, InetSockAddress& localAddr)
    {
        auto length = static_cast<SockLenType>(sizeof(SockAddr6));
        return ::accept4(sockFd, localAddr.sockAddr(), &length, SOCK_NONBLOCK | SOCK_CLOEXEC);
    }

    ::int32_t connect(FdType sockFd, const InetSockAddress& peerAddr)
    {
        auto length = static_cast<SockLenType>((peerAddr.ipFamily() == AF_INET ? sizeof(SockAddr4) : sizeof(SockAddr6)));
        return ::connect(sockFd, peerAddr.sockAddr(), length);
    }

    SSizeType read(FdType fd, void* buf, SizeType n)
    {
        return ::read(fd, buf, n);
    }

    SSizeType readv(FdType fd, const IoVec* iov, ::int32_t iovcnt)
    {
        return ::readv(fd, iov, iovcnt);
    }

    SSizeType write(FdType fd, const void* buf, SizeType n)
    {
        return ::write(fd, buf, n);
    }

    SSizeType writev(FdType fd, const IoVec* iov, ::int32_t iovcnt)
    {
        return ::writev(fd, iov, iovcnt);
    }

    SSizeType sendTo(FdType fd, const void* buf, SizeType n, const InetSockAddress& destAddr)
    {
        auto length = static_cast<SockLenType>((destAddr.ipFamily() == AF_INET ? sizeof(SockAddr4) : sizeof(SockAddr6)));
        return ::sendto(fd, buf, n, 0, destAddr.sockAddr(), length);
    }

    SSizeType recvFrom(FdType fd, void* buf, SizeType n, InetSockAddress& srcAddr)
    {
        auto length = static_cast<SockLenType>(sizeof(SockAddr6));
        return ::recvfrom(fd, buf, n, 0, srcAddr.sockAddr(), &length);
    }

    void getLocalAddress(FdType fd, InetSockAddress& sockAddr)
    {
        auto length = static_cast<SockLenType>(sizeof(SockAddr6));
        if (0 != ::getsockname(fd, sockAddr.sockAddr(), &length))
        {
            LOGS_ERROR << "socket getLocalAddress failed";
        }
    }

    void getPeerAddress(FdType fd, InetSockAddress& sockAddr)
    {
        auto length = static_cast<SockLenType>(sizeof(SockAddr6));
        if (0 != ::getpeername(fd, sockAddr.sockAddr(), &length))
        {
            LOGS_ERROR << "socket getPeerAddress failed";
        }
    }

    OptValType getSockError(FdType sockFd)
    {
        OptValType optVal = 0;
        auto length = static_cast<SockLenType>(sizeof(OptValType));
        if (0 != ::getsockopt(sockFd, SOL_SOCKET, SO_ERROR, &optVal, &length))
        {
            LOGS_ERROR << "socket getSockError failed";
            return errno;
        }
        else
        {
            return optVal;
        }
    }

    void setSockReuseAddr(FdType sockFd, bool enable)
    {
        OptValType reuse = enable ? 1 : 0;
        if (0 != ::setsockopt(sockFd, SOL_SOCKET, SO_REUSEADDR, &reuse, static_cast<SockLenType>(sizeof(OptValType))))
        {
            LOGS_ERROR << "socket setSockAddrReuse failed";
        }
    }

    void setSockReusePort(FdType sockFd, bool enable)
    {
        OptValType reuse = enable ? 1 : 0;
        if (0 != ::setsockopt(sockFd, SOL_SOCKET, SO_REUSEPORT, &reuse, static_cast<SockLenType>(sizeof(OptValType))))
        {
            LOGS_ERROR << "socket setSockPortReuse failed";
        }
    }

    void setSockKeepAlive(FdType sockFd, bool enable)
    {
        OptValType keepAlive = enable ? 1 : 0;
        if (0 != ::setsockopt(sockFd, SOL_SOCKET, SO_KEEPALIVE, &keepAlive, static_cast<SockLenType>(sizeof(OptValType))))
        {
            LOGS_ERROR << "socket setSockKeepAlive failed";
        }
    }

    void setSockKeepIdle(FdType sockFd, OptValType idleTime)
    {
        if (0 != ::setsockopt(sockFd, IPPROTO_TCP, TCP_KEEPIDLE, &idleTime, static_cast<SockLenType>(sizeof(OptValType))))
        {
            LOGS_ERROR << "socket setSockKeepIdle failed";
        }
    }

    void setSockKeepCnt(FdType sockFd, OptValType cnt)
    {
        if (0 != ::setsockopt(sockFd, IPPROTO_TCP, TCP_KEEPCNT, &cnt, static_cast<SockLenType>(sizeof(OptValType))))
        {
            LOGS_ERROR << "socket setSockKeepCnt failed";
        }
    }

    void setSockKeepIntvl(FdType sockFd, OptValType interval)
    {
        if (0 != ::setsockopt(sockFd, IPPROTO_TCP, TCP_KEEPINTVL, &interval, static_cast<SockLenType>(sizeof(OptValType))))
        {
            LOGS_ERROR << "socket setSockKeepIntvl failed";
        }
    }

    void setTcpNoDelay(FdType sockFd, bool enable)
    {
        OptValType noDelay = enable ? 1 : 0;
        if (0 != ::setsockopt(sockFd, IPPROTO_TCP, TCP_NODELAY, &noDelay, static_cast<SockLenType>(sizeof(OptValType))))
        {
            LOGS_ERROR << "socket setTcpNoDelay failed";
        }
    }

    void setSockNonBlock(FdType sockFd, bool enable)
    {
        ::int32_t flags = ::fcntl(sockFd, F_GETFL, 0);
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

    void setSockLinger(FdType sockFd, const SockLinger& linger)
    {
        if (0 != ::setsockopt(sockFd, SOL_SOCKET, SO_LINGER, &linger, static_cast<SockLenType>(sizeof(SockLinger))))
        {
            LOGS_ERROR << "socket setSockLinger failed";
        }
    }

    void setSockBroadCast(FdType sockFd, bool enable)
    {
        OptValType broadcast = enable ? 1 : 0;
        if (0 != ::setsockopt(sockFd, SOL_SOCKET, SO_BROADCAST, &broadcast, static_cast<SockLenType>(sizeof(OptValType))))
        {
            LOGS_ERROR << "socket setSockBroadCast failed";
        }
    }

    void setSockSendBuf(FdType sockFd, OptValType sendBufLen)
    {
        if (0 != ::setsockopt(sockFd, SOL_SOCKET, SO_SNDBUF, &sendBufLen, static_cast<SockLenType>(sizeof(OptValType))))
        {
            LOGS_ERROR << "socket setSockSendBuf failed";
        }
    }

    void setSockRecvBuf(FdType sockFd, OptValType recvBufLen)
    {
        if (0 != ::setsockopt(sockFd, SOL_SOCKET, SO_RCVBUF, &recvBufLen, static_cast<SockLenType>(sizeof(OptValType))))
        {
            LOGS_ERROR << "socket setSockRecvBuf failed";
        }
    }

    OptValType getTcpSockSendBuf()
    {
        FdType sockFd = socket::createTcpSocket(AF_INET);
        OptValType optVal = 0;
        auto length = static_cast<SockLenType>(sizeof(OptValType));
        if (0 != ::getsockopt(sockFd, SOL_SOCKET, SO_SNDBUF, &optVal, &length))
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
        auto length = static_cast<SockLenType>(sizeof(OptValType));
        if (0 != ::getsockopt(sockFd, SOL_SOCKET, SO_RCVBUF, &optVal, &length))
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
        auto length = static_cast<SockLenType>(sizeof(OptValType));
        if (0 != ::getsockopt(sockFd, SOL_SOCKET, SO_SNDBUF, &optVal, &length))
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
        auto length = static_cast<SockLenType>(sizeof(OptValType));
        if (0 != ::getsockopt(sockFd, SOL_SOCKET, SO_RCVBUF, &optVal, &length))
        {
            LOGS_ERROR << "socket getUdpSockRecvBuf failed";
        }
        socket::closeFd(sockFd);
        return optVal;
    }
} // namespace nets::net::socket