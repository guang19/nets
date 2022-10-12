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

#ifndef NETS_INET_SOCK_ADDRESS_H
#define NETS_INET_SOCK_ADDRESS_H

#include <netinet/in.h>
#include <string>

#include "nets/base/Copyable.h"

namespace nets
{
    namespace
    {
        using SockLenType = ::socklen_t;
        using PortType = ::uint16_t;
        using SockAddr = struct sockaddr;
        using SockAddr4 = struct sockaddr_in;
        using SockAddr6 = struct sockaddr_in6;
        using SockAddrFamily = ::sa_family_t;
    } // namespace

    class InetSockAddress : public Copyable
    {
    public:
        using StringType = ::std::string;

    public:
        InetSockAddress();
        explicit InetSockAddress(const SockAddr4& addr4);
        explicit InetSockAddress(const SockAddr6& addr6);
        explicit InetSockAddress(const StringType& ip, PortType port, bool ipv6 = false);
        ~InetSockAddress() = default;

        InetSockAddress(const InetSockAddress& other);
        InetSockAddress(InetSockAddress&& other) noexcept;
        InetSockAddress& operator=(const InetSockAddress& other);
        InetSockAddress& operator=(InetSockAddress&& other) noexcept;

    public:
        static InetSockAddress createAnySockAddress(PortType port, bool ipv6 = false);
        static InetSockAddress createLoopBackSockAddress(PortType port, bool ipv6 = false);

    public:
        inline SockAddrFamily ipFamily() const
        {
            return addr_.sa_family;
        }

        inline const SockAddr* sockAddr() const
        {
            return &addr_;
        }

        inline SockAddr* sockAddr()
        {
            return &addr_;
        }

        inline const SockAddr4* sockAddr4() const
        {
            return &addr4_;
        }

        inline SockAddr4* sockAddr4()
        {
            return &addr4_;
        }

        inline const SockAddr6* sockAddr6() const
        {
            return &addr6_;
        }

        inline SockAddr6* sockAddr6()
        {
            return &addr6_;
        }

        StringType ip() const;
        PortType port() const;
        StringType toString() const;
        bool isInValid() const;

    private:
        union
        {
            SockAddr addr_;
            SockAddr4 addr4_;
            SockAddr6 addr6_;
        };
    };
} // namespace nets

#endif // NETS_INET_SOCK_ADDRESS_H