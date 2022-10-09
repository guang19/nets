//
// Created by guang19
//

#include "nets/net/core/SocketChannelHandlerPipeline.h"

namespace nets
{
    SocketChannelHandlerPipeline::SocketChannelHandlerPipeline(SocketChannelContextRawPtr channelContext)
        : channelContext_(channelContext), channelHandlers_()
    {
    }

    SocketChannelHandlerPipeline::SocketChannelHandlerPipeline(const SocketChannelHandlerPipeline& other)
    {
        this->channelHandlers_ = other.channelHandlers_;
    }

    SocketChannelHandlerPipeline::SocketChannelHandlerPipeline(SocketChannelHandlerPipeline&& other) noexcept
        : channelHandlers_(::std::move(other.channelHandlers_))
    {
    }

    SocketChannelHandlerPipeline& SocketChannelHandlerPipeline::operator=(const SocketChannelHandlerPipeline& other)
    {
        if (this != &other)
        {
            this->channelHandlers_ = other.channelHandlers_;
        }
        return *this;
    }

    SocketChannelHandlerPipeline& SocketChannelHandlerPipeline::operator=(SocketChannelHandlerPipeline&& other) noexcept
    {
        if (this != &other)
        {
            this->channelHandlers_ = ::std::move(other.channelHandlers_);
        }
        return *this;
    }

    void SocketChannelHandlerPipeline::addFirst(SocketChannelHandlerRawPtr channelHandler)
    {
        channelHandlers_.push_front(SocketChannelHandlerPtr(channelHandler));
    }

    void SocketChannelHandlerPipeline::addFirst(const SocketChannelHandlerPtr& channelHandler)
    {
        channelHandlers_.push_front(channelHandler);
    }

    void SocketChannelHandlerPipeline::addLast(SocketChannelHandlerRawPtr channelHandler)
    {
        channelHandlers_.push_back(SocketChannelHandlerPtr(channelHandler));
    }

    void SocketChannelHandlerPipeline::addLast(const SocketChannelHandlerPtr& channelHandler)
    {
        channelHandlers_.push_back(channelHandler);
    }

    void SocketChannelHandlerPipeline::fireSocketChannelConnect(const InetSockAddress& localAddress,
                                                                const InetSockAddress& peerAddress)
    {
        for (auto& channelHandler : channelHandlers_)
        {
            channelHandler->channelConnect(*channelContext_, localAddress, peerAddress);
        }
    }

    void SocketChannelHandlerPipeline::fireSocketChannelDisconnect()
    {
        for (auto& channelHandler : channelHandlers_)
        {
            channelHandler->channelDisconnect(*channelContext_);
        }
    }

    void SocketChannelHandlerPipeline::fireSocketChannelRead(ByteBuffer& message)
    {
        for (auto& channelHandler : channelHandlers_)
        {
            channelHandler->channelRead(*channelContext_, message);
        }
    }
} // namespace nets