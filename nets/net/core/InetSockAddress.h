//
// Created by guang19 on 2022/5/19.
//

#ifndef NETS_NET_INETSOCK_ADDRESS_H
#define NETS_NET_INETSOCK_ADDRESS_H

#include <netinet/in.h>
#include <string>

#include "nets/base/Copyable.h"

namespace nets::net
{
    namespace
    {
        using SockLenType = ::socklen_t;
        using PortType = uint16_t;
        using SockAddr = struct sockaddr;
        using SockAddr4 = struct sockaddr_in;
        using SockAddr6 = struct sockaddr_in6;
        using SockAddrFamily = ::sa_family_t;
    } // namespace

    class InetSockAddress : public nets::base::Copyable
    {
    public:
        InetSockAddress();
        explicit InetSockAddress(const SockAddr4& addr4);
        explicit InetSockAddress(const SockAddr6& addr6);
        explicit InetSockAddress(const char* ip, PortType port, bool ipv6 = false);
        ~InetSockAddress() = default;

        InetSockAddress(const InetSockAddress& addr);
        InetSockAddress(InetSockAddress&& addr) noexcept;
        InetSockAddress& operator=(const InetSockAddress& addr);
        InetSockAddress& operator=(InetSockAddress&& addr) noexcept;

    public:
        static InetSockAddress createAnySockAddress(PortType port, bool ipv6 = false);
        static InetSockAddress createLoopBackSockAddress(PortType port, bool ipv6 = false);

    public:
        inline SockAddrFamily ipFamily() const
        {
            return addr_.sa_family;
        }

        inline const SockAddr* csockAddr() const
        {
            return &addr_;
        }

        inline SockAddr* sockAddr()
        {
            return &addr_;
        }

        ::std::string ip() const;
        PortType port() const;
        ::std::string toString() const;

    private:
        union
        {
            SockAddr addr_;
            SockAddr4 addr4_;
            SockAddr6 addr6_;
        };
    };
} // namespace nets::net

#endif // NETS_NET_INETSOCK_ADDRESS_H
