//
// Created by guang19
//

#include "nets/net/core/Socket.h"

#include <arpa/inet.h>
#include <fcntl.h>
#include <net/if.h>
#include <netinet/tcp.h>
#include <sys/uio.h>
#include <unistd.h>

#include "nets/base/log/Logger.h"
#include "nets/net/exception/SocketCreationException.h"
#include "nets/net/exception/SocketOperationException.h"

namespace nets::socket
{
    FdType createTcpSocket(SockAddrFamily family)
    {
        FdType sockFd = ::socket(family, SOCK_STREAM, IPPROTO_TCP);
        if (sockFd < 0)
        {
            THROW_FMT(SocketCreationException, "socket createTcpSocket failed,errno=%d", errno);
        }
        setSockCloExec(sockFd);
        return sockFd;
    }

    FdType createUdpSocket(SockAddrFamily family)
    {
        FdType sockFd = ::socket(family, SOCK_DGRAM, IPPROTO_UDP);
        if (sockFd < 0)
        {
            THROW_FMT(SocketCreationException, "socket createUdpSocket failed,errno=%d", errno);
        }
        setSockCloExec(sockFd);
        return sockFd;
    }

    void setSockCloExec(FdType sockFd)
    {
        ::int32_t flags = ::fcntl(sockFd, F_GETFD, 0);
        flags |= FD_CLOEXEC;
        if (::fcntl(sockFd, F_SETFD, flags) == -1)
        {
            THROW_FMT(SocketOperationException, "socket set FD_CLOEXEC failed,errno=%d", errno);
        }
    }

    void closeFd(FdType fd)
    {
        if (fd >= 0)
        {
            if (::close(fd) == -1)
            {
                NETS_SYSTEM_LOG_ERROR << "socket closeFd " << fd << " failed,errno=" << errno;
            }
        }
    }

    void shutdown(FdType sockFd, ::int32_t how)
    {
        if (::shutdown(sockFd, how) == -1)
        {
            NETS_SYSTEM_LOG_ERROR << "socket shutdown " << shutdownHowToString(how) << " failed,errno=" << errno;
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
            NETS_SYSTEM_LOG_ERROR << "socket createIdleFd failed,errno=" << errno;
            return socket::kInvalidFd;
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
        if (::bind(sockFd, localAddr.sockAddr(), length) == -1)
        {
            THROW_FMT(SocketOperationException, "socket bind failed,errno=%d", errno);
        }
    }

    void listen(FdType sockFd, ::int32_t backlog)
    {
        if (::listen(sockFd, backlog) == -1)
        {
            THROW_FMT(SocketOperationException, "socket listen failed,errno=%d", errno);
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
        if (::getsockname(fd, sockAddr.sockAddr(), &length) == -1)
        {
            NETS_SYSTEM_LOG_ERROR << "socket getLocalAddress failed,errno=" << errno;
        }
    }

    void getPeerAddress(FdType fd, InetSockAddress& sockAddr)
    {
        auto length = static_cast<SockLenType>(sizeof(SockAddr6));
        if (::getpeername(fd, sockAddr.sockAddr(), &length) == -1)
        {
            NETS_SYSTEM_LOG_ERROR << "socket getPeerAddress failed,errno=" << errno;
        }
    }

    OptValType getSockError(FdType sockFd)
    {
        OptValType optVal = 0;
        auto length = static_cast<SockLenType>(sizeof(OptValType));
        if (::getsockopt(sockFd, SOL_SOCKET, SO_ERROR, &optVal, &length) == -1)
        {
            int32_t errNum = errno;
            NETS_SYSTEM_LOG_ERROR << "socket getSockError failed,errno=" << errNum;
            return errNum;
        }
        else
        {
            return optVal;
        }
    }

    void setSockReuseAddr(FdType sockFd, bool enable)
    {
        OptValType reuse = enable ? 1 : 0;
        if (::setsockopt(sockFd, SOL_SOCKET, SO_REUSEADDR, &reuse, static_cast<SockLenType>(sizeof(OptValType))) == -1)
        {
            NETS_SYSTEM_LOG_ERROR << "socket setSockAddrReuse failed,errno=" << errno;
        }
    }

    void setSockReusePort(FdType sockFd, bool enable)
    {
        OptValType reuse = enable ? 1 : 0;
        if (::setsockopt(sockFd, SOL_SOCKET, SO_REUSEPORT, &reuse, static_cast<SockLenType>(sizeof(OptValType))) == -1)
        {
            NETS_SYSTEM_LOG_ERROR << "socket setSockPortReuse failed,errno=" << errno;
        }
    }

    void setSockKeepAlive(FdType sockFd, bool enable)
    {
        OptValType keepalive = enable ? 1 : 0;
        if (::setsockopt(sockFd, SOL_SOCKET, SO_KEEPALIVE, &keepalive, static_cast<SockLenType>(sizeof(OptValType))) == -1)
        {
            NETS_SYSTEM_LOG_ERROR << "socket setSockKeepAlive failed,errno=" << errno;
        }
    }

    void setSockKeepIdle(FdType sockFd, OptValType idleTime)
    {
        if (::setsockopt(sockFd, IPPROTO_TCP, TCP_KEEPIDLE, &idleTime, static_cast<SockLenType>(sizeof(OptValType))) == -1)
        {
            NETS_SYSTEM_LOG_ERROR << "socket setSockKeepIdle failed,errno=" << errno;
        }
    }

    void setSockKeepCnt(FdType sockFd, OptValType cnt)
    {
        if (::setsockopt(sockFd, IPPROTO_TCP, TCP_KEEPCNT, &cnt, static_cast<SockLenType>(sizeof(OptValType))) == -1)
        {
            NETS_SYSTEM_LOG_ERROR << "socket setSockKeepCnt failed,errno=" << errno;
        }
    }

    void setSockKeepIntvl(FdType sockFd, OptValType interval)
    {
        if (::setsockopt(sockFd, IPPROTO_TCP, TCP_KEEPINTVL, &interval, static_cast<SockLenType>(sizeof(OptValType))) == -1)
        {
            NETS_SYSTEM_LOG_ERROR << "socket setSockKeepIntvl failed,errno=" << errno;
        }
    }

    void setTcpNoDelay(FdType sockFd, bool enable)
    {
        OptValType noDelay = enable ? 1 : 0;
        if (::setsockopt(sockFd, IPPROTO_TCP, TCP_NODELAY, &noDelay, static_cast<SockLenType>(sizeof(OptValType))) == -1)
        {
            NETS_SYSTEM_LOG_ERROR << "socket setTcpNoDelay failed,errno=" << errno;
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
        if (::fcntl(sockFd, F_SETFL, flags) == -1)
        {
            NETS_SYSTEM_LOG_ERROR << "socket setSockNonBlock failed,errno=" << errno;
        }
    }

    void setSockLinger(FdType sockFd, const SockLinger& linger)
    {
        if (::setsockopt(sockFd, SOL_SOCKET, SO_LINGER, &linger, static_cast<SockLenType>(sizeof(SockLinger))) == -1)
        {
            NETS_SYSTEM_LOG_ERROR << "socket setSockLinger failed,errno=" << errno;
        }
    }

    void setSockBroadcast(FdType sockFd, bool enable)
    {
        OptValType broadcast = enable ? 1 : 0;
        if (::setsockopt(sockFd, SOL_SOCKET, SO_BROADCAST, &broadcast, static_cast<SockLenType>(sizeof(OptValType))) == -1)
        {
            NETS_SYSTEM_LOG_ERROR << "socket setSockBroadcast failed,errno=" << errno;
        }
    }

    void setIpMulticastIf(FdType sockFd, const StringType& ifAddr)
    {
        InAddr inf {0};
        ::inet_pton(AF_INET, ifAddr.data(), &inf.s_addr);
        if (::setsockopt(sockFd, IPPROTO_IP, IP_MULTICAST_IF, &inf, static_cast<SockLenType>(sizeof(InAddr))) == -1)
        {
            NETS_SYSTEM_LOG_ERROR << "socket setIpMulticastIf failed,errno=" << errno;
        }
    }

    void setIpv6MulticastIf(FdType sockFd, const StringType& ifName)
    {
        uint32_t ifIndex = ::if_nametoindex(ifName.data());
        if (::setsockopt(sockFd, IPPROTO_IPV6, IPV6_MULTICAST_IF, &ifIndex, static_cast<SockLenType>(sizeof(uint32_t))) ==
            -1)
        {
            NETS_SYSTEM_LOG_ERROR << "socket setIpv6MulticastIf failed,errno=" << errno;
        }
    }

    void setIpMulticastTTL(FdType sockFd, uint8_t ttl)
    {
        if (::setsockopt(sockFd, IPPROTO_IP, IP_MULTICAST_IF, &ttl, static_cast<SockLenType>(sizeof(uint8_t))) == -1)
        {
            NETS_SYSTEM_LOG_ERROR << "socket setIpMulticastTTL failed,errno=" << errno;
        }
    }

    void setIpv6MulticastHops(FdType sockFd, uint8_t hops)
    {
        if (::setsockopt(sockFd, IPPROTO_IPV6, IPV6_MULTICAST_HOPS, &hops, static_cast<SockLenType>(sizeof(uint8_t))) == -1)
        {
            NETS_SYSTEM_LOG_ERROR << "socket setIpv6MulticastHops failed,errno=" << errno;
        }
    }

    void setIpMulticastLoop(FdType sockFd, bool enable)
    {
        if (::setsockopt(sockFd, IPPROTO_IP, IP_MULTICAST_LOOP, &enable, static_cast<SockLenType>(sizeof(bool))) == -1)
        {
            NETS_SYSTEM_LOG_ERROR << "socket setIpMulticastLoop failed,errno=" << errno;
        }
    }

    void setIpv6MulticastLoop(FdType sockFd, bool enable)
    {
        if (::setsockopt(sockFd, IPPROTO_IPV6, IPV6_MULTICAST_LOOP, &enable, static_cast<SockLenType>(sizeof(bool))) == -1)
        {
            NETS_SYSTEM_LOG_ERROR << "socket setIpv6MulticastLoop failed,errno=" << errno;
        }
    }

    bool addIpMemberShipByIfAddr(FdType sockFd, const StringType& multicastAddr, const StringType& ifAddr)
    {
        IpMreqn group {};
        ::inet_pton(AF_INET, multicastAddr.data(), &group.imr_multiaddr.s_addr);
        ::inet_pton(AF_INET, ifAddr.data(), &group.imr_address.s_addr);
        // find internet interface index
        group.imr_ifindex = 0;
        if (::setsockopt(sockFd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &group, static_cast<SockLenType>(sizeof(IpMreqn))) == -1)
        {
            NETS_SYSTEM_LOG_ERROR << "socket addIpMemberShipByIfAddr failed,errno=" << errno;
            return false;
        }
        return true;
    }

    bool addIpMemberShipByIfIndex(FdType sockFd, const StringType& multicastAddr, const StringType& inf)
    {
        IpMreqn group {};
        ::inet_pton(AF_INET, multicastAddr.data(), &group.imr_multiaddr.s_addr);
        group.imr_address.s_addr = htonl(INADDR_ANY);
        // find internet interface index
        group.imr_ifindex = static_cast<int32_t>(::if_nametoindex(inf.data()));
        if (::setsockopt(sockFd, IPPROTO_IP, IPV6_ADD_MEMBERSHIP, &group, static_cast<SockLenType>(sizeof(IpMreqn))) == -1)
        {
            NETS_SYSTEM_LOG_ERROR << "socket addIpMemberShipByIfIndex failed,errno=" << errno;
            return false;
        }
        return true;
    }

    bool addIpv6MemberShip(FdType sockFd, const StringType& multicastAddr, const StringType& inf)
    {
        Ipv6Mreq group {};
        ::inet_pton(AF_INET6, multicastAddr.data(), &group.ipv6mr_multiaddr);
        group.ipv6mr_interface = ::if_nametoindex(inf.data());
        if (::setsockopt(sockFd, IPPROTO_IPV6, IPV6_JOIN_GROUP /*IPV6_ADD_MEMBERSHIP*/, &group,
                         static_cast<SockLenType>(sizeof(Ipv6Mreq))) == -1)
        {
            NETS_SYSTEM_LOG_ERROR << "socket addIpv6MemberShip failed,errno=" << errno;
            return false;
        }
        return true;
    }

    bool dropIpMemberShipByIfAddr(FdType sockFd, const StringType& multicastAddr, const StringType& ifAddr)
    {
        IpMreqn group {};
        ::inet_pton(AF_INET, multicastAddr.data(), &group.imr_multiaddr.s_addr);
        ::inet_pton(AF_INET, ifAddr.data(), &group.imr_address.s_addr);
        // find internet interface index
        group.imr_ifindex = 0;
        if (::setsockopt(sockFd, IPPROTO_IP, IP_DROP_MEMBERSHIP, &group, static_cast<SockLenType>(sizeof(IpMreqn))) == -1)
        {
            NETS_SYSTEM_LOG_ERROR << "socket dropIpMemberShipByIfAddr failed,errno=" << errno;
            return false;
        }
        return true;
    }

    bool dropIpMemberShipByIfIndex(FdType sockFd, const StringType& multicastAddr, const StringType& inf)
    {
        IpMreqn group {};
        ::inet_pton(AF_INET, multicastAddr.data(), &group.imr_multiaddr.s_addr);
        group.imr_address.s_addr = htonl(INADDR_ANY);
        // find internet interface index
        group.imr_ifindex = static_cast<int32_t>(::if_nametoindex(inf.data()));
        if (::setsockopt(sockFd, IPPROTO_IP, IP_DROP_MEMBERSHIP, &group, static_cast<SockLenType>(sizeof(IpMreqn))) == -1)
        {
            NETS_SYSTEM_LOG_ERROR << "socket dropIpMemberShipByIfIndex failed,errno=" << errno;
            return false;
        }
        return true;
    }

    bool dropIpv6MemberShip(FdType sockFd, const StringType& multicastAddr, const StringType& inf)
    {
        Ipv6Mreq group {};
        ::inet_pton(AF_INET6, multicastAddr.data(), &group.ipv6mr_multiaddr);
        group.ipv6mr_interface = ::if_nametoindex(inf.data());
        if (::setsockopt(sockFd, IPPROTO_IPV6, IPV6_LEAVE_GROUP /*IPV6_DROP_MEMBERSHIP*/, &group,
                         static_cast<SockLenType>(sizeof(Ipv6Mreq))) == -1)
        {
            NETS_SYSTEM_LOG_ERROR << "socket dropIpv6MemberShip failed,errno=" << errno;
            return false;
        }
        return true;
    }

    void setSockSendBuf(FdType sockFd, OptValType sendBufLen)
    {
        if (::setsockopt(sockFd, SOL_SOCKET, SO_SNDBUF, &sendBufLen, static_cast<SockLenType>(sizeof(OptValType))) == -1)
        {
            NETS_SYSTEM_LOG_ERROR << "socket setSockSendBuf failed,errno=" << errno;
        }
    }

    void setSockRecvBuf(FdType sockFd, OptValType recvBufLen)
    {
        if (::setsockopt(sockFd, SOL_SOCKET, SO_RCVBUF, &recvBufLen, static_cast<SockLenType>(sizeof(OptValType))) == -1)
        {
            NETS_SYSTEM_LOG_ERROR << "socket setSockRecvBuf failed,errno=" << errno;
        }
    }

    OptValType getTcpSockSendBuf()
    {
        FdType sockFd = socket::createTcpSocket(AF_INET);
        OptValType optVal = 0;
        auto length = static_cast<SockLenType>(sizeof(OptValType));
        if (::getsockopt(sockFd, SOL_SOCKET, SO_SNDBUF, &optVal, &length) == -1)
        {
            NETS_SYSTEM_LOG_ERROR << "socket getTcpSockSendBuf failed,errno=" << errno;
        }
        socket::closeFd(sockFd);
        return optVal;
    }

    OptValType getTcpSockRecvBuf()
    {
        FdType sockFd = socket::createTcpSocket(AF_INET);
        OptValType optVal = 0;
        auto length = static_cast<SockLenType>(sizeof(OptValType));
        if (::getsockopt(sockFd, SOL_SOCKET, SO_RCVBUF, &optVal, &length) == -1)
        {
            NETS_SYSTEM_LOG_ERROR << "socket getTcpSockRecvBuf failed,errno=" << errno;
        }
        socket::closeFd(sockFd);
        return optVal;
    }

    OptValType getUdpSockSendBuf()
    {
        FdType sockFd = socket::createUdpSocket(AF_INET);
        OptValType optVal = 0;
        auto length = static_cast<SockLenType>(sizeof(OptValType));
        if (::getsockopt(sockFd, SOL_SOCKET, SO_SNDBUF, &optVal, &length) == -1)
        {
            NETS_SYSTEM_LOG_ERROR << "socket getUdpSockSendBuf failed,errno=" << errno;
        }
        socket::closeFd(sockFd);
        return optVal;
    }

    OptValType getUdpSockRecvBuf()
    {
        FdType sockFd = socket::createUdpSocket(AF_INET);
        OptValType optVal = 0;
        auto length = static_cast<SockLenType>(sizeof(OptValType));
        if (::getsockopt(sockFd, SOL_SOCKET, SO_RCVBUF, &optVal, &length) == -1)
        {
            NETS_SYSTEM_LOG_ERROR << "socket getUdpSockRecvBuf failed,errno=" << errno;
        }
        socket::closeFd(sockFd);
        return optVal;
    }
} // namespace nets::socket