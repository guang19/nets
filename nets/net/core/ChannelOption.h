//
// Created by guang19 on 2022/7/25.
//

#ifndef NETS_CHANNEL_OPTION_H
#define NETS_CHANNEL_OPTION_H

#include "nets/base/Noncopyable.h"
#include "nets/net/core/Socket.h"

namespace nets::net
{
    enum SockOpt
    {
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

    template <typename ValueType>
    class ChannelOption : nets::base::Noncopyable
    {
    public:
        explicit ChannelOption(SockOpt sockOpt, const ValueType& value) : sockOpt_(sockOpt), value_(value) {}
        ~ChannelOption() = default;

    public:
        inline const ValueType& get()
        {
            return value_;
        }

        inline void set(const ValueType& value)
        {
            value_ = value;
        }

    private:
        SockOpt sockOpt_ {};
        ValueType value_;
    };

    namespace
    {
        const ChannelOption<int32_t> NBackLog {SockOpt::NBACKLOG, SOMAXCONN};
        const ChannelOption<bool> NKeepAlive {SockOpt::NKEEPALIVE, true};
        const ChannelOption<bool> NReuseAddr {SockOpt::NREUSEADDR, true};
        const ChannelOption<bool> NReusePort {SockOpt::NREUSEPORT, true};
        const ChannelOption<bool> NTcpNoDelay {SockOpt::TCPNODELAY, false};
        const ChannelOption<int32_t> NLinger {SockOpt::LINGER, 0};
        const ChannelOption<int32_t> NTcpSendBuf {SockOpt::TCPSNDBUF, DefaultTcpSockSendBufLen};
        const ChannelOption<int32_t> NTcpRecvBuf {SockOpt::TCPRCVBUF, DefaultTcpSockRecvBufLen};
        const ChannelOption<int32_t> NUdpSendBuf {SockOpt::UDPSNDBUF, DefaultUdpSockSendBufLen};
        const ChannelOption<int32_t> NUdpRecvBuf {SockOpt::UDPRCVBUF, DefaultUdpSockRecvBufLen};
    } // namespace
} // namespace nets::net

#endif // NETS_CHANNEL_OPTION_H
