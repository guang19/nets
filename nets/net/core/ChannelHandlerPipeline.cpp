//
// Created by guang19
//

#include "nets/net/core/ChannelHandlerPipeline.h"

namespace nets::net
{
    ChannelHandlerPipeline::ChannelHandlerPipeline(ChannelContextRawPtr channelContext)
        : channelContext_(channelContext), channelHandlers_()
    {
    }

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

    void ChannelHandlerPipeline::fireSocketChannelConnect(const InetSockAddress& localAddress,
                                                          const InetSockAddress& peerAddress)
    {
        for (auto& channelHandler: channelHandlers_)
        {
            auto handler = ::std::dynamic_pointer_cast<SocketChannelHandler>(channelHandler);
            handler->channelConnect(dynamic_cast<SocketChannelContext&>(*channelContext_), localAddress, peerAddress);
        }
    }

    void ChannelHandlerPipeline::fireSocketChannelDisconnect()
    {
        for (auto& channelHandler: channelHandlers_)
        {
            auto handler = ::std::dynamic_pointer_cast<SocketChannelHandler>(channelHandler);
            handler->channelDisconnect(dynamic_cast<SocketChannelContext&>(*channelContext_));
        }
    }

    void ChannelHandlerPipeline::fireSocketChannelRead(ByteBuffer& message)
    {
        for (auto& channelHandler: channelHandlers_)
        {
            auto handler = ::std::dynamic_pointer_cast<SocketChannelHandler>(channelHandler);
            handler->channelRead(dynamic_cast<SocketChannelContext&>(*channelContext_), message);
        }
    }

    void ChannelHandlerPipeline::fireDatagramChannelActive()
    {
        for (auto& channelHandler: channelHandlers_)
        {
            auto handler = ::std::dynamic_pointer_cast<DatagramChannelHandler>(channelHandler);
            handler->channelActive(dynamic_cast<DatagramChannelContext&>(*channelContext_));
        }
    }

    void ChannelHandlerPipeline::fireDatagramChannelRead(DatagramPacket& message)
    {
        for (auto& channelHandler: channelHandlers_)
        {
            auto handler = ::std::dynamic_pointer_cast<DatagramChannelHandler>(channelHandler);
            handler->channelRead(dynamic_cast<DatagramChannelContext&>(*channelContext_), message);
        }
    }
} // namespace nets::net