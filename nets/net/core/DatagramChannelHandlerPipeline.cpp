// MIT License
//
// Copyright (c) 2022 guang19
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

// @brief Manage DatagramChannelHandler owned by DatagramChannel

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