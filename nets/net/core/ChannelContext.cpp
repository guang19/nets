//
// Created by guang19 on 2022/6/28.
//

#include "nets/net/core/ChannelContext.h"

#include "nets/net/core/Channel.h"

namespace nets::net
{
    ChannelContext::ChannelContext(ChannelRawPtr channel) : channel_(channel) {}
} // namespace nets::net