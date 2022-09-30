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
        BACKLOG = 0,        // int32_t
        REUSE_ADDR,         // bool
        REUSE_PORT,         // bool
        KEEP_ALIVE,         // bool
        KEEP_IDLE,          // int32_t, unit:seconds
        KEEP_CNT,           // int32_t
        KEEP_INTVL,         // int32_t, unit:seconds
        TCP_NODELAY,        // bool
        LINGER,             // struct linger
        BROADCAST,          // bool
        IP4_MULTICAST_IF,   // string
        IP6_MULTICAST_IF,   // string
        IP4_MULTICAST_TTL,  // uint8_t
        IP6_MULTICAST_HOPS, // uint8_t
        IP4_MULTICAST_LOOP, // bool
        IP6_MULTICAST_LOOP, // bool
        SNDBUF,             // int32_t
        RCVBUF,             // int32_t
    };

    namespace ChannelOption
    {
        using ValueType = ::std::variant<bool, uint8_t, OptValType, SockLinger, ::std::string>;
    }
} // namespace nets::net

#endif // NETS_NET_CHANNEL_OPTION_H
