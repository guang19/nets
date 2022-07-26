//
// Created by guang19 on 2022/7/25.
//

#ifndef NETS_CHANNEL_OPTION_H
#define NETS_CHANNEL_OPTION_H

#include <any>
#include <variant>

#include "nets/base/Noncopyable.h"
#include "nets/net/core/Socket.h"

namespace nets::net
{
    enum SockOpt
    {
        InvalidSockOpt = -1,
        NBACKLOG = 0,
        NKEEPALIVE,
        NREUSEADDR,
        NREUSEPORT,
        TCPNODELAY,
        LINGER,
        TCPSNDBUF,
        TCPRCVBUF,
        UDPSNDBUF,
        UDPRCVBUF,
    };

    class ChannelOption : nets::base::Copyable
    {
    public:
        using ValueType = ::std::variant<bool, int32_t>;

    public:
        struct ChannelOptionHasher
        {
            ::size_t operator()(const ChannelOption& channelOption) const;
        };

    public:
        ChannelOption();
        explicit ChannelOption(SockOpt sockOpt, const ValueType& value);
        ~ChannelOption() = default;

        ChannelOption(const ChannelOption& other);
        ChannelOption(ChannelOption&& other) noexcept;
        ChannelOption& operator=(const ChannelOption& other);
        ChannelOption& operator=(ChannelOption&& other) noexcept;

        bool operator==(const ChannelOption& other) const;
        bool operator!=(const ChannelOption& other) const;
        bool operator<(const ChannelOption& other) const;
        bool operator>(const ChannelOption& other) const;

    public:
        bool valid() const;
        ::std::any get() const;
        void set(const ValueType& value);

    private:
        SockOpt sockOpt_ {SockOpt::InvalidSockOpt};
        ValueType value_ {};
    };

    namespace
    {
        const ChannelOption NReuseAddr {SockOpt::NREUSEADDR, true};
        const ChannelOption NReusePort {SockOpt::NREUSEPORT, true};
        const ChannelOption NKeepAlive {SockOpt::NKEEPALIVE, false};
        const ChannelOption NTcpNoDelay {SockOpt::TCPNODELAY, false};
        const ChannelOption NLinger {SockOpt::LINGER, DefaultSockLinger};
        const ChannelOption NBackLog {SockOpt::NBACKLOG, DefaultMaximumOfBackLog};
        const ChannelOption NTcpSendBuf {SockOpt::TCPSNDBUF, DefaultTcpSockSendBufLen};
        const ChannelOption NTcpRecvBuf {SockOpt::TCPRCVBUF, DefaultTcpSockRecvBufLen};
        const ChannelOption NUdpSendBuf {SockOpt::UDPSNDBUF, DefaultUdpSockSendBufLen};
        const ChannelOption NUdpRecvBuf {SockOpt::UDPRCVBUF, DefaultUdpSockRecvBufLen};
    } // namespace
} // namespace nets::net

#endif // NETS_CHANNEL_OPTION_H
