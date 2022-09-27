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
    enum SockOption
    {
        INVALID_SOCKOPT = -1,
        BACKLOG = 0, // int32_t
        REUSE_ADDR,  // bool
        REUSE_PORT,  // bool
        KEEP_ALIVE,  // bool
        TCP_NODELAY, // bool
        LINGER,      // struct linger
        BROADCAST,   // bool
        SNDBUF,      // int32_t
        RCVBUF,      // int32_t
    };

    namespace ChannelOption
    {
        using ValueType = ::std::variant<bool, ::int32_t, SockLinger>;
    }
} // namespace nets::net

#endif // NETS_NET_CHANNEL_OPTION_H
