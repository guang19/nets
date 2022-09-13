//
// Created by guang19
//

#include "nets/net/core/ChannelContext.h"

#include <utility>

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

    void ChannelContext::write(const void* message, SizeType length, WriteCompleteCallback writeCompleteCallback)
    {
        channel_->write(message, length, std::move(writeCompleteCallback));
    }

    void ChannelContext::write(const StringType& message, WriteCompleteCallback writeCompleteCallback)
    {
        channel_->write(message, std::move(writeCompleteCallback));
    }

    void ChannelContext::write(const ByteBuffer& message, WriteCompleteCallback writeCompleteCallback)
    {
        channel_->write(message, std::move(writeCompleteCallback));
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