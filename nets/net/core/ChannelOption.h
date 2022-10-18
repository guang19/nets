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

// @brief Socket options

#ifndef NETS_CHANNEL_OPTION_H
#define NETS_CHANNEL_OPTION_H

#include <variant>

#include "nets/base/Noncopyable.h"
#include "nets/net/core/Socket.h"

namespace nets
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
        using ValueType = ::std::variant<bool, Uint8Type, OptValType, SockLinger, StringType>;
    }
} // namespace nets

#endif // NETS_CHANNEL_OPTION_H
