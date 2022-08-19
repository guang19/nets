//
// Created by guang19
//

#include "nets/net/core/ChannelContext.h"

#include "nets/net/core/SocketChannel.h"

namespace nets::net
{
    ChannelContext::ChannelContext(ChannelRawPtr channel) : channel_(channel) {}

    const InetSockAddress& ChannelContext::localAddress() const
    {
        return channel_->localAddress();
    }

    const InetSockAddress& ChannelContext::peerAddress() const
    {
        return channel_->peerAddress();
    }

    void ChannelContext::write(const void* message, SizeType length)
    {
        channel_->write(message, length);
    }

    void ChannelContext::write(const StringType& message)
    {
        channel_->write(message);
    }

    void ChannelContext::write(const ByteBuffer& message)
    {
        channel_->write(message);
    }

    bool ChannelContext::isActive() const
    {
        return channel_->isActive();
    }

    void ChannelContext::disconnect()
    {
        shutdown();
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