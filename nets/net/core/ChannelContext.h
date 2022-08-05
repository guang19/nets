//
// Created by guang19 on 2022/6/28.
//

#ifndef NETS_NET_CHANNEL_CONTEXT_H
#define NETS_NET_CHANNEL_CONTEXT_H

#include "nets/base/Noncopyable.h"
#include "nets/net/core/Channel.h"

namespace nets::net
{
    class ChannelContext : nets::base::Noncopyable
    {
    public:
        using ChannelRawPtr = Channel*;

    public:
        explicit ChannelContext(ChannelRawPtr channel);
        ~ChannelContext() = default;

    public:
        inline ChannelRawPtr channel()
        {
            return channel_;
        }

    private:
        ChannelRawPtr channel_ {nullptr};
    };
} // namespace nets::net

#endif // NETS_NET_CHANNEL_CONTEXT_H
