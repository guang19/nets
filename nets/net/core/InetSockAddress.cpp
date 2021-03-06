//
// Created by guang19 on 2022/5/19.
//

#include "nets/net/core/InetSockAddress.h"

#include <arpa/inet.h>

#include "nets/base/CommonMacro.h"
#include "nets/base/log/Logging.h"

namespace nets::net
{
    InetSockAddress::InetSockAddress()
    {
        MEMZERO(&addr6_, sizeof(SockAddr6));
    }

    InetSockAddress::InetSockAddress(const SockAddr4& addr4)
    {
        MEMZERO(&addr6_, sizeof(SockAddr6));
        addr4_ = addr4;
    }

    InetSockAddress::InetSockAddress(const SockAddr6& addr6)
    {
        MEMZERO(&addr6_, sizeof(SockAddr6));
        addr6_ = addr6;
    }

    InetSockAddress::InetSockAddress(const char* ip, PortType port, bool ipv6)
    {
        MEMZERO(&addr6_, sizeof(SockAddr6));
        if (ipv6)
        {
            addr6_.sin6_family = AF_INET6;
            addr6_.sin6_port = htobe16(port);
            if (1 != ::inet_pton(AF_INET6, ip, &addr6_.sin6_addr))
            {
                LOGS_FATAL << "InetSockAddress inet_pton AF_INET6 " << ip;
            }
        }
        else
        {
            addr4_.sin_family = AF_INET;
            addr4_.sin_port = htobe16(port);
            if (1 != ::inet_pton(AF_INET, ip, &(addr4_.sin_addr)))
            {
                LOGS_FATAL << "InetSockAddress inet_pton AF_INET " << ip;
            }
        }
    }

    InetSockAddress::InetSockAddress(const InetSockAddress& other)
    {
        MEMZERO(&addr6_, sizeof(SockAddr6));
        if (AF_INET6 == other.ipFamily())
        {
            addr6_ = other.addr6_;
        }
        else if (AF_INET == other.ipFamily())
        {
            addr4_ = other.addr4_;
        }
    }

    InetSockAddress::InetSockAddress(InetSockAddress&& other) noexcept
    {
        MEMZERO(&addr6_, sizeof(SockAddr6));
        if (AF_INET6 == other.ipFamily())
        {
            addr6_ = other.addr6_;
        }
        else if (AF_INET == other.ipFamily())
        {
            addr4_ = other.addr4_;
        }
        MEMZERO(&other.addr6_, sizeof(SockAddr6));
    }

    InetSockAddress& InetSockAddress::operator=(const InetSockAddress& other)
    {
        if (this != &other)
        {
            MEMZERO(&addr6_, sizeof(SockAddr6));
            if (AF_INET6 == other.ipFamily())
            {
                addr6_ = other.addr6_;
            }
            else if (AF_INET == other.ipFamily())
            {
                addr4_ = other.addr4_;
            }
        }
        return *this;
    }

    InetSockAddress& InetSockAddress::operator=(InetSockAddress&& other) noexcept
    {
        if (this != &other)
        {
            MEMZERO(&addr6_, sizeof(SockAddr6));
            if (AF_INET6 == other.ipFamily())
            {
                addr6_ = other.addr6_;
            }
            else if (AF_INET == other.ipFamily())
            {
                addr4_ = other.addr4_;
            }
            MEMZERO(&other.addr6_, sizeof(SockAddr6));
        }
        return *this;
    }

    InetSockAddress InetSockAddress::createAnySockAddress(PortType port, bool ipv6)
    {
        if (ipv6)
        {
            SockAddr6 addr {};
            MEMZERO(&addr, sizeof(SockAddr6));
            addr.sin6_family = AF_INET6;
            addr.sin6_port = htobe16(port);
            addr.sin6_addr = in6addr_any;
            return InetSockAddress(addr);
        }
        else
        {
            SockAddr4 addr {};
            MEMZERO(&addr, sizeof(SockAddr4));
            addr.sin_family = AF_INET;
            addr.sin_port = htobe16(port);
            addr.sin_addr.s_addr = INADDR_ANY;
            return InetSockAddress(addr);
        }
    }

    InetSockAddress InetSockAddress::createLoopBackSockAddress(PortType port, bool ipv6)
    {
        if (ipv6)
        {
            SockAddr6 addr {};
            MEMZERO(&addr, sizeof(SockAddr6));
            addr.sin6_family = AF_INET6;
            addr.sin6_port = htobe16(port);
            addr.sin6_addr = in6addr_loopback;
            return InetSockAddress(addr);
        }
        else
        {
            SockAddr4 addr {};
            MEMZERO(&addr, sizeof(SockAddr4));
            addr.sin_family = AF_INET;
            addr.sin_port = htobe16(port);
            addr.sin_addr.s_addr = INADDR_LOOPBACK;
            return InetSockAddress(addr);
        }
    }
    
    ::std::string InetSockAddress::ip() const
    {
        char buffer[64] = {0};
        auto len = static_cast<SockLenType>(sizeof(buffer));
        if (AF_INET6 == addr_.sa_family)
        {
            if (nullptr == ::inet_ntop(AF_INET6, &addr6_.sin6_addr, buffer, len))
            {
                LOGS_FATAL << "InetSockAddress inet_ntop AF_INET6 failed";
            }
        }
        else if (AF_INET == addr_.sa_family)
        {
            if (nullptr == ::inet_ntop(AF_INET, &addr4_.sin_addr, buffer, len))
            {
                LOGS_FATAL << "InetSockAddress inet_ntop AF_INET failed";
            }
        }
        return buffer;
    }

    PortType InetSockAddress::port() const
    {
        return be16toh(addr4_.sin_port);
    }

    ::std::string InetSockAddress::toString() const
    {
        char buffer[64] = {0};
        auto len = static_cast<SockLenType>(sizeof(buffer));
        if (AF_INET6 == addr_.sa_family)
        {
            // ipv6 + port address like: [2a01:198:603:0:396e]:8080
            buffer[0] = '[';
            if (nullptr == ::inet_ntop(AF_INET6, &addr6_.sin6_addr, buffer + 1, len))
            {
                LOGS_FATAL << "InetSockAddress inet_ntop AF_INET6 failed";
            }
            SockLenType ipLen = ::strlen(buffer);
            PortType port = be16toh(addr6_.sin6_port);
            ::snprintf(buffer + ipLen, len - ipLen, "]:%u", port);
        }
        else if (AF_INET == addr_.sa_family)
        {
            if (nullptr == ::inet_ntop(AF_INET, &addr4_.sin_addr, buffer, len))
            {
                LOGS_FATAL << "InetSockAddress inet_ntop AF_INET failed";
            }
            SockLenType ipLen = ::strlen(buffer);
            PortType port = be16toh(addr4_.sin_port);
            ::snprintf(buffer + ipLen, len - ipLen, ":%u", port);
        }
        return buffer;
    }
} // namespace nets::net