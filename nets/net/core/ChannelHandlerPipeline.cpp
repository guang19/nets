//
// Created by guang19
//

#include "nets/net/core/ChannelHandlerPipeline.h"

namespace nets::net
{
    ChannelHandlerPipeline::ChannelHandlerPipeline(ChannelRawPtr channel) : channelContext_(channel), channelHandlers_() {}

    ChannelHandlerPipeline::ChannelHandlerPipeline(const ChannelHandlerPipeline& other)
    {
        this->channelHandlers_ = other.channelHandlers_;
    }

    ChannelHandlerPipeline::ChannelHandlerPipeline(ChannelHandlerPipeline&& other) noexcept
    {
        this->channelHandlers_ = ::std::move(other.channelHandlers_);
    }

    ChannelHandlerPipeline& ChannelHandlerPipeline::operator=(const ChannelHandlerPipeline& other)
    {
        if (this != &other)
        {
            this->channelHandlers_ = other.channelHandlers_;
        }
        return *this;
    }

    ChannelHandlerPipeline& ChannelHandlerPipeline::operator=(ChannelHandlerPipeline&& other) noexcept
    {
        if (this != &other)
        {
            this->channelHandlers_ = ::std::move(other.channelHandlers_);
        }
        return *this;
    }

    void ChannelHandlerPipeline::addFirst(ChannelHandlerRawPtr channelHandler)
    {
        channelHandlers_.push_front(ChannelHandlerPtr(channelHandler));
    }

    void ChannelHandlerPipeline::addFirst(const ChannelHandlerPtr& channelHandler)
    {
        channelHandlers_.push_front(channelHandler);
    }

    void ChannelHandlerPipeline::addLast(ChannelHandlerRawPtr channelHandler)
    {
        channelHandlers_.push_back(ChannelHandlerPtr(channelHandler));
    }

    void ChannelHandlerPipeline::addLast(const ChannelHandlerPtr& channelHandler)
    {
        channelHandlers_.push_back(channelHandler);
    }

    void ChannelHandlerPipeline::addFirst(SocketChannelHandlerRawPtr channelHandler)
    {
        channelHandlers_.push_front(ChannelHandlerPtr(channelHandler));
    }

    void ChannelHandlerPipeline::addFirst(const SocketChannelHandlerPtr& channelHandler)
    {
        channelHandlers_.push_front(channelHandler);
    }

    void ChannelHandlerPipeline::addLast(SocketChannelHandlerRawPtr channelHandler)
    {
        channelHandlers_.push_back(ChannelHandlerPtr(channelHandler));
    }

    void ChannelHandlerPipeline::addLast(const SocketChannelHandlerPtr& channelHandler)
    {
        channelHandlers_.push_back(channelHandler);
    }

    void ChannelHandlerPipeline::fireChannelConnect(const InetSockAddress& localAddress, const InetSockAddress& peerAddress)
    {
        for (auto& channelHandler: channelHandlers_)
        {
            auto handler = ::std::dynamic_pointer_cast<SocketChannelHandler>(channelHandler);
            try
            {
                handler->channelConnect(channelContext_, localAddress, peerAddress);
            }
            catch (const ::std::exception& exception)
            {
                handler->exceptionCaught(channelContext_, exception);
            }
        }
    }

    void ChannelHandlerPipeline::fireChannelDisconnect()
    {
        for (auto& channelHandler: channelHandlers_)
        {
            auto handler = ::std::dynamic_pointer_cast<SocketChannelHandler>(channelHandler);
            try
            {
                handler->channelDisconnect(channelContext_);
            }
            catch (const ::std::exception& exception)
            {
                handler->exceptionCaught(channelContext_, exception);
            }
        }
    }

    void ChannelHandlerPipeline::fireChannelRead(ByteBuffer& message)
    {
        for (auto& channelHandler: channelHandlers_)
        {
            auto handler = ::std::dynamic_pointer_cast<SocketChannelHandler>(channelHandler);
            try
            {
                handler->channelRead(channelContext_, message);
            }
            catch (const ::std::exception& exception)
            {
                handler->exceptionCaught(channelContext_, exception);
            }
        }
    }

    void ChannelHandlerPipeline::fireChannelWriteComplete()
    {
        for (auto& channelHandler: channelHandlers_)
        {
            auto handler = ::std::dynamic_pointer_cast<SocketChannelHandler>(channelHandler);
            try
            {
                handler->channelWriteComplete(channelContext_);
            }
            catch (const ::std::exception& exception)
            {
                handler->exceptionCaught(channelContext_, exception);
            }
        }
    }
} // namespace nets::net