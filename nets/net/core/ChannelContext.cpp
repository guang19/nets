//
// Created by guang19
//

#include "nets/net/core/ChannelContext.h"

#include "nets/net/core/SocketChannel.h"

namespace nets::net
{
    ChannelContext::ChannelContext(ChannelRawPtr channel) : channel_(channel) {}

    bool ChannelContext::isActive() const
    {
        return channel_->isActive();
    }

    void ChannelContext::shutdown()
    {
        channel_->shutdown();
    }

    void ChannelContext::shutdownRead()
    {
        channel_->shutdownRead();
    }

    void ChannelContext::shutdownWrite()
    {
        channel_->shutdownWrite();
    }
} // namespace nets::net