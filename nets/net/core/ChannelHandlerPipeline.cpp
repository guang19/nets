//
// Created by guang19 on 2022/6/26.
//

#include "nets/net/core/ChannelHandlerPipeline.h"

namespace nets::net
{
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

    void ChannelHandlerPipeline::addFirst(ChannelHandlerPtr channelHandler)
    {
        channelHandlers_.push_front(channelHandler);
    }

    void ChannelHandlerPipeline::addLast(ChannelHandlerPtr channelHandler)
    {
        channelHandlers_.push_back(channelHandler);
    }
} // namespace nets::net