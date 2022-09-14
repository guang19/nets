//
// Created by guang19
//

#ifndef NETS_NET_CHANNEL_CONTEXT_H
#define NETS_NET_CHANNEL_CONTEXT_H

#include "nets/base/Noncopyable.h"

namespace nets::net
{
    class ChannelContext : nets::base::Noncopyable
    {
    public:
        ChannelContext() = default;
        virtual ~ChannelContext() = default;
    };
} // namespace nets::net

#endif // NETS_NET_CHANNEL_CONTEXT_H