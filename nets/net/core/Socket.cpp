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
            LOGS_FATAL << "socket createTcpSocket failed";
        }
        setSockCloExec(sockFd);
        return sockFd;
    }

    FdType createUdpSocket(SockAddrFamily family)
    {
        FdType sockFd = ::socket(family, SOCK_DGRAM, IPPROTO_UDP);
        if (sockFd < 0)
        {
            LOGS_FATAL << "socket createUdpSocket failed";
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
            LOGS_FATAL << "socket set FD_CLOEXEC failed";
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

    void dealwithEMFILE(FdType* idleFd, FdType sockFd)
    {
        closeFd(*idleFd);
        FdType connFd = ::accept(sockFd, nullptr, nullptr);
        // close client connection, avoid listenFd always trigger
        closeFd(connFd);
        // recreate idle fd
        *idleFd = createIdleFd();
    }

    void bind(FdType sockFd, const SockAddr* sockAddr)
    {
        auto len = static_cast<SockLenType>((sockAddr->sa_family == AF_INET ? sizeof(SockAddr4) : sizeof(SockAddr6)));
        if (0 != ::bind(sockFd, sockAddr, len))
        {
            LOGS_FATAL << "socket bind failed";
        }
    }

    void connect(FdType sockFd, const SockAddr* sockAddr)
    {
        auto len = static_cast<SockLenType>((sockAddr->sa_family == AF_INET ? sizeof(SockAddr4) : sizeof(SockAddr6)));
        if (0 != connect(sockFd, sockAddr, len))
        {
            LOGS_ERROR << "socket connect failed";
        }
    }

    void listen(FdType sockFd, int32_t backlog)
    {
        if (0 != ::listen(sockFd, backlog))
        {
            LOGS_FATAL << "socket listen failed";
        }
    }

    FdType accept(FdType sockFd, SockAddr* sockAddr, FdType* idleFd)
    {
        auto len = static_cast<SockLenType>(sizeof(SockAddr6));
        FdType connFd = socket::InvalidFd;
        while (true)
        {
            if ((connFd = ::accept4(sockFd, sockAddr, &len, SOCK_NONBLOCK | SOCK_CLOEXEC)) >= 0)
            {
                return connFd;
            }
            else
            {
                LOGS_ERROR << "socket accept failed";
                int32_t errorN = errno;
                switch (errorN)
                {
                    // EAGAIN/EWOULDBLOCK is not an error
                    // EWOULDBLOCK
                    case EAGAIN:
                        LOGS_WARN << "socket accpet EAGAIN";
                        break;
                    // retry
                    case EINTR:
                    case ECONNABORTED:
                        MEMZERO(sockAddr, len);
                        continue;
                    // the per-process limit on the number of open file descriptors has been reached
                    case EMFILE:
                    {
                        if (idleFd != nullptr && *idleFd >= 0)
                        {
                            dealwithEMFILE(idleFd, sockFd);
                        }
                        LOGS_ERROR << "socket accpet EAGAIN";
                        break;
                    }
                    // error
                    case EBADF:
                    case EFAULT:
                    case EINVAL:
                    case ENFILE:  // the system-wide limit on the total number of open files has been reached
                    case ENOBUFS: // Not enough free memory
                    case ENOMEM:  // Not enough free memory
                    case ENOTSOCK:
                    case EOPNOTSUPP:
                    case EPERM:
                    case EPROTO:
                    default:
                        LOGS_FATAL << "socket accept unexpected error:" << errorN;
                        break;
                }
            }
        }
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
        ::getsockopt(sockFd, SOL_SOCKET, SO_SNDBUF, &optVal, &len);
        socket::closeFd(sockFd);
        return optVal;
    }

    OptValType getTcpSockRecvBuf()
    {
        FdType sockFd = socket::createTcpSocket(AF_INET);
        OptValType optVal = 0;
        auto len = static_cast<SockLenType>(sizeof(optVal));
        ::getsockopt(sockFd, SOL_SOCKET, SO_RCVBUF, &optVal, &len);
        socket::closeFd(sockFd);
        return optVal;
    }

    OptValType getUdpSockSendBuf()
    {
        FdType sockFd = socket::createUdpSocket(AF_INET);
        OptValType optVal = 0;
        auto len = static_cast<SockLenType>(sizeof(optVal));
        ::getsockopt(sockFd, SOL_SOCKET, SO_SNDBUF, &optVal, &len);
        socket::closeFd(sockFd);
        return optVal;
    }

    OptValType getUdpSockRecvBuf()
    {
        FdType sockFd = socket::createUdpSocket(AF_INET);
        OptValType optVal = 0;
        auto len = static_cast<SockLenType>(sizeof(optVal));
        ::getsockopt(sockFd, SOL_SOCKET, SO_RCVBUF, &optVal, &len);
        socket::closeFd(sockFd);
        return optVal;
    }

    void setSockReuseAddr(FdType sockFd, bool enable)
    {
        OptValType reuse = enable ? 1 : 0;
        if (0 != ::setsockopt(sockFd, SOL_SOCKET, SO_REUSEADDR, &reuse, static_cast<SockLenType>(sizeof(reuse))))
        {
            LOGS_FATAL << "socket setSockAddrReuse failed";
        }
    }

    void setSockReusePort(FdType sockFd, bool enable)
    {
        OptValType reuse = enable ? 1 : 0;
        if (0 != ::setsockopt(sockFd, SOL_SOCKET, SO_REUSEPORT, &reuse, static_cast<SockLenType>(sizeof(reuse))))
        {
            LOGS_FATAL << "socket setSockPortReuse failed";
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
        if (0 != ::setsockopt(sockFd, IPPROTO_TCP, SO_LINGER, &linger, static_cast<SockLenType>(sizeof(SockLinger))))
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