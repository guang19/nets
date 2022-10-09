//
// Created by guang19
//

#include "nets/net/core/DatagramChannelHandlerPipeline.h"

namespace nets
{
    DatagramChannelHandlerPipeline::DatagramChannelHandlerPipeline(DatagramChannelContextRawPtr channelContext)
        : channelContext_(channelContext), channelHandlers_()
    {
    }

    DatagramChannelHandlerPipeline::DatagramChannelHandlerPipeline(const DatagramChannelHandlerPipeline& other)
    {
        this->channelHandlers_ = other.channelHandlers_;
    }

    DatagramChannelHandlerPipeline::DatagramChannelHandlerPipeline(DatagramChannelHandlerPipeline&& other) noexcept
        : channelHandlers_(::std::move(other.channelHandlers_))
    {
    }

    DatagramChannelHandlerPipeline& DatagramChannelHandlerPipeline::operator=(const DatagramChannelHandlerPipeline& other)
    {
        if (this != &other)
        {
            this->channelHandlers_ = other.channelHandlers_;
        }
        return *this;
    }

    DatagramChannelHandlerPipeline&
    DatagramChannelHandlerPipeline::operator=(DatagramChannelHandlerPipeline&& other) noexcept
    {
        if (this != &other)
        {
            this->channelHandlers_ = ::std::move(other.channelHandlers_);
        }
        return *this;
    }

    void DatagramChannelHandlerPipeline::addFirst(DatagramChannelHandlerRawPtr channelHandler)
    {
        channelHandlers_.push_front(DatagramChannelHandlerPtr(channelHandler));
    }

    void DatagramChannelHandlerPipeline::addFirst(const DatagramChannelHandlerPtr& channelHandler)
    {
        channelHandlers_.push_front(channelHandler);
    }

    void DatagramChannelHandlerPipeline::addLast(DatagramChannelHandlerRawPtr channelHandler)
    {
        channelHandlers_.push_back(DatagramChannelHandlerPtr(channelHandler));
    }

    void DatagramChannelHandlerPipeline::addLast(const DatagramChannelHandlerPtr& channelHandler)
    {
        channelHandlers_.push_back(channelHandler);
    }

    void DatagramChannelHandlerPipeline::fireDatagramChannelActive()
    {
        for (auto& channelHandler : channelHandlers_)
        {
            channelHandler->channelActive(*channelContext_);
        }
    }

    void DatagramChannelHandlerPipeline::fireDatagramChannelRead(DatagramPacket& message)
    {
        for (auto& channelHandler : channelHandlers_)
        {
            channelHandler->channelRead(*channelContext_, message);
        }
    }
} // namespace nets