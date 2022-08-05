//
// Created by guang19 on 2022/7/25.
//

#ifndef NETS_NET_CHANNEL_OPTION_H
#define NETS_NET_CHANNEL_OPTION_H

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
        NREUSEADDR,
        NREUSEPORT,
        NKEEPALIVE,
        NTCPNODELAY,
        NLINGER,
        NTCPSNDBUF,
        NTCPRCVBUF,
        NUDPSNDBUF,
        NUDPRCVBUF,
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
        inline bool valid() const
        {
            return sockOpt_ != SockOpt::InvalidSockOpt;
        }

        inline SockOpt sockOpt() const
        {
            return sockOpt_;
        }

        ::std::any get() const;

        inline void set(const ValueType& value)
        {
            value_ = value;
        }

    private:
        SockOpt sockOpt_ {SockOpt::InvalidSockOpt};
        ValueType value_ {};
    };

    namespace
    {
        const ChannelOption NReuseAddr {SockOpt::NREUSEADDR, true};
        const ChannelOption NReusePort {SockOpt::NREUSEPORT, true};
        const ChannelOption NKeepAlive {SockOpt::NKEEPALIVE, false};
        const ChannelOption NTcpNoDelay {SockOpt::NTCPNODELAY, false};
        const ChannelOption NLinger {SockOpt::NLINGER, DefaultSockLinger};
        const ChannelOption NBackLog {SockOpt::NBACKLOG, DefaultMaximumOfBackLog};
        const ChannelOption NTcpSendBuf {SockOpt::NTCPSNDBUF, DefaultTcpSockSendBufSize};
        const ChannelOption NTcpRecvBuf {SockOpt::NTCPRCVBUF, DefaultTcpSockRecvBufSize};
        const ChannelOption NUdpSendBuf {SockOpt::NUDPSNDBUF, DefaultUdpSockSendBufSize};
        const ChannelOption NUdpRecvBuf {SockOpt::NUDPRCVBUF, DefaultUdpSockRecvBufSize};
    } // namespace
} // namespace nets::net

#endif // NETS_NET_CHANNEL_OPTION_H
