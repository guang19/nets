//
// Created by guang19
//

#ifndef NETS_NET_CHANNEL_OPTION_H
#define NETS_NET_CHANNEL_OPTION_H

#include <variant>

#include "nets/base/Noncopyable.h"
#include "nets/net/core/Socket.h"

namespace nets::net
{
    enum SockOption
    {
        INVALID_SOCKOPT = -1,
        BACKLOG = 0,     // int32_t
        REUSE_ADDR,      // bool
        REUSE_PORT,      // bool
        KEEP_ALIVE,      // bool
        KEEP_IDLE,       // int32_t, unit:seconds
        KEEP_CNT,        // int32_t
        KEEP_INTVL,      // int32_t, unit:seconds
        TCP_NODELAY,     // bool
        LINGER,          // struct linger
        BROADCAST,       // bool
        MULTICAST4_IF,   // string
        MULTICAST6_IF,   // string
        MULTICAST4_LOOP, // bool
        MULTICAST6_LOOP, // bool
        MULTICAST_TTL,   // int32_t
        SNDBUF,          // int32_t
        RCVBUF,          // int32_t
    };

    namespace ChannelOption
    {
        using ValueType = ::std::variant<bool, OptValType, ::std::string, SockLinger>;
    }
} // namespace nets::net

#endif // NETS_NET_CHANNEL_OPTION_H
