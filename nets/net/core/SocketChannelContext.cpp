//
// Created by guang19
//

#include "nets/net/core/SocketChannelContext.h"

#include "nets/net/core/SocketChannel.h"

namespace nets::net
{
    SocketChannelContext::SocketChannelContext(SocketChannelRawPtr channel) : channel_(channel) {}

    const InetSockAddress& SocketChannelContext::localAddress() const
    {
        return channel_->localAddress();
    }

    const InetSockAddress& SocketChannelContext::peerAddress() const
    {
        return channel_->peerAddress();
    }

    void SocketChannelContext::write(const void* message, SizeType length, WriteCompleteCallback writeCompleteCallback)
    {
        channel_->write(message, length, std::move(writeCompleteCallback));
    }

    void SocketChannelContext::write(const StringType& message, WriteCompleteCallback writeCompleteCallback)
    {
        channel_->write(message, std::move(writeCompleteCallback));
    }

    void SocketChannelContext::write(const ByteBuffer& message, WriteCompleteCallback writeCompleteCallback)
    {
        channel_->write(message, std::move(writeCompleteCallback));
    }

    bool SocketChannelContext::isActive() const
    {
        return channel_->isActive();
    }

    void SocketChannelContext::disconnect()
    {
        shutdown();
    }

    void SocketChannelContext::shutdown()
    {
        channel_->shutdown();
    }

    void SocketChannelContext::shutdownRead()
    {
        channel_->shutdownRead();
    }

    void SocketChannelContext::shutdownWrite()
    {
        channel_->shutdownWrite();
    }
} // namespace nets::net