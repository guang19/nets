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
        BACKLOG = 0,
        REUSE_ADDR,
        REUSE_PORT,
        KEEP_ALIVE,
        TCP_NODELAY,
        LINGER,
        BROADCAST,
        TCP_SNDBUF,
        TCP_RCVBUF,
        UDP_SNDBUF,
        UDP_RCVBUF,
    };

    class ChannelOption : nets::base::Copyable
    {
    public:
        using ValueType = ::std::variant<bool, ::int32_t>;

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
        const ChannelOption SO_BackLog {SockOpt::BACKLOG, DefaultMaximumOfBackLog};
        const ChannelOption SO_ReuseAddr {SockOpt::REUSE_ADDR, true};
        const ChannelOption SO_ReusePort {SockOpt::REUSE_PORT, true};
        const ChannelOption SO_KeepAlive {SockOpt::KEEP_ALIVE, false};
        const ChannelOption SO_TcpNoDelay {SockOpt::TCP_NODELAY, false};
        const ChannelOption SO_Linger {SockOpt::LINGER, DefaultSockLinger};
        const ChannelOption SO_BroadCast {SockOpt::BROADCAST, true};
        const ChannelOption SO_TcpSendBuffer {SockOpt::TCP_SNDBUF, DefaultTcpSockSendBufferSize};
        const ChannelOption SO_TcpRecvBuffer {SockOpt::TCP_RCVBUF, DefaultTcpSockRecvBufferSize};
        const ChannelOption SO_UdpSendBuffer {SockOpt::UDP_SNDBUF, DefaultUdpSockSendBufferSize};
        const ChannelOption SO_UdpRecvBuffer {SockOpt::UDP_RCVBUF, DefaultUdpSockRecvBufferSize};
    } // namespace
} // namespace nets::net

#endif // NETS_NET_CHANNEL_OPTION_H
