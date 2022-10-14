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

// @brief The encapsulation of network addresses

#include "nets/net/core/InetSockAddress.h"

#include <arpa/inet.h>

#include "nets/base/Common.h"
#include "nets/base/log/Logger.h"
#include "nets/net/exception/InetSockAddressException.h"

namespace nets
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

    InetSockAddress::InetSockAddress(const StringType& ip, PortType port, bool ipv6)
    {
        MEMZERO(&addr6_, sizeof(SockAddr6));
        if (ipv6)
        {
            addr6_.sin6_family = AF_INET6;
            addr6_.sin6_port = htobe16(port);
            if (::inet_pton(AF_INET6, ip.data(), &addr6_.sin6_addr) == -1)
            {
                THROW_FMT(InetSockAddressException, "InetSockAddress inet_pton AF_INET6,ip=%s", ip.data());
            }
        }
        else
        {
            addr4_.sin_family = AF_INET;
            addr4_.sin_port = htobe16(port);
            if (::inet_pton(AF_INET, ip.data(), &(addr4_.sin_addr)) == -1)
            {
                THROW_FMT(InetSockAddressException, "InetSockAddress inet_pton AF_INET,ip=%s", ip.data());
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
            addr.sin_addr.s_addr = htonl(INADDR_ANY);
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
            addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
            return InetSockAddress(addr);
        }
    }

    InetSockAddress::StringType InetSockAddress::ip() const
    {
        char buffer[64] = {0};
        auto length = static_cast<SockLenType>(sizeof(buffer));
        if (AF_INET6 == addr_.sa_family)
        {
            if (::inet_ntop(AF_INET6, &addr6_.sin6_addr, buffer, length) == nullptr)
            {
                THROW_FMT(InetSockAddressException, "InetSockAddress inet_ntop AF_INET6 failed,errno=%d", errno);
            }
        }
        else if (AF_INET == addr_.sa_family)
        {
            if (::inet_ntop(AF_INET, &addr4_.sin_addr, buffer, length) == nullptr)
            {
                THROW_FMT(InetSockAddressException, "InetSockAddress inet_ntop AF_INET failed,errno=%d", errno);
            }
        }
        return buffer;
    }

    PortType InetSockAddress::port() const
    {
        return be16toh(addr4_.sin_port);
    }

    InetSockAddress::StringType InetSockAddress::toString() const
    {
        char buffer[128] = {0};
        auto length = static_cast<SockLenType>(sizeof(buffer));
        if (AF_INET6 == addr_.sa_family)
        {
            // ipv6 + port address like: [2a01:198:603:0:396e]:8080
            buffer[0] = '[';
            if (::inet_ntop(AF_INET6, &addr6_.sin6_addr, buffer + 1, length) == nullptr)
            {
                THROW_FMT(InetSockAddressException, "InetSockAddress inet_ntop AF_INET6 failed,errno=%d", errno);
            }
            SockLenType ipLen = ::strlen(buffer);
            PortType port = be16toh(addr6_.sin6_port);
            ::snprintf(buffer + ipLen, length - ipLen, "]:%u", port);
        }
        else if (AF_INET == addr_.sa_family)
        {
            if (::inet_ntop(AF_INET, &addr4_.sin_addr, buffer, length) == nullptr)
            {
                THROW_FMT(InetSockAddressException, "InetSockAddress inet_ntop AF_INET faile0,errno=%d", errno);
            }
            SockLenType ipLen = ::strlen(buffer);
            PortType port = be16toh(addr4_.sin_port);
            ::snprintf(buffer + ipLen, length - ipLen, ":%u", port);
        }
        return buffer;
    }

    bool InetSockAddress::isInValid() const
    {
        return addr_.sa_family == AF_UNSPEC;
    }
} // namespace nets