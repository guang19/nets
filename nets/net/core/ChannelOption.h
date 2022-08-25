//
// Created by guang19
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
        INVALID_SOCKOPT = -1,
        NBACKLOG = 0,
        NREUSE_ADDR,
        NREUSE_PORT,
        NKEEP_ALIVE,
        NTCP_NODELAY,
        NLINGER,
        NTCP_SNDBUF,
        NTCP_RCVBUF,
        NUDP_SNDBUF,
        NUDP_RCVBUF,
    };

    class ChannelOption : nets::base::Copyable
    {
    public:
        using ValueType = ::std::variant<bool, ::int32_t>;

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
            return sockOpt_ != SockOpt::INVALID_SOCKOPT;
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
        SockOpt sockOpt_ {SockOpt::INVALID_SOCKOPT};
        ValueType value_ {};
    };

    namespace
    {
        const ChannelOption NReuseAddr {SockOpt::NREUSE_ADDR, true};
        const ChannelOption NReusePort {SockOpt::NREUSE_PORT, true};
        const ChannelOption NKeepAlive {SockOpt::NKEEP_ALIVE, false};
        const ChannelOption NTcpNoDelay {SockOpt::NTCP_NODELAY, false};
        const ChannelOption NLinger {SockOpt::NLINGER, DefaultSockLinger};
        const ChannelOption NBackLog {SockOpt::NBACKLOG, DefaultMaximumOfBackLog};
        const ChannelOption NTcpSendBuffer {SockOpt::NTCP_SNDBUF, DefaultTcpSockSendBufferSize};
        const ChannelOption NTcpRecvBuffer {SockOpt::NTCP_RCVBUF, DefaultTcpSockRecvBufferSize};
        const ChannelOption NUdpSendBuffer {SockOpt::NUDP_SNDBUF, DefaultUdpSockSendBufferSize};
        const ChannelOption NUdpRecvBuffer {SockOpt::NUDP_RCVBUF, DefaultUdpSockRecvBufferSize};
    } // namespace
} // namespace nets::net

#endif // NETS_NET_CHANNEL_OPTION_H
