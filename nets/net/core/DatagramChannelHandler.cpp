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

// @brief The DatagramChannel event handler

#include "nets/net/core/DatagramChannelHandler.h"

#include "nets/base/log/Logger.h"
#include "nets/net/core/DatagramChannelContext.h"

namespace nets
{
    DatagramChannelHandler::DatagramChannelHandler() : name_(), next_(nullptr) {}

    DatagramChannelHandler::DatagramChannelHandler(const StringType& name) : name_(name), next_(nullptr) {}

    void DatagramChannelHandler::addLast(const DatagramChannelHandlerPtr& channelHandler)
    {
        if (next_ != nullptr)
        {
            auto temp = next_;
            while (temp->next_ != nullptr)
            {
                temp = temp->next_;
            }
            temp->next_ = channelHandler;
        }
        else
        {
            next_ = channelHandler;
        }
    }

    void DatagramChannelHandler::channelActive(DatagramChannelContext& channelContext)
    {
        NETS_SYSTEM_LOG_DEBUG << "DatagramChannelHandler::channelActive";
        fireChannelActive(channelContext);
    }

    void DatagramChannelHandler::channelRead(DatagramChannelContext& channelContext, DatagramPacket& message)
    {
        NETS_SYSTEM_LOG_DEBUG << "DatagramChannelHandler::channelRead";
        fireChannelRead(channelContext, message);
    }

    void DatagramChannelHandler::fireChannelActive(DatagramChannelContext& channelContext)
    {
        if (next_ != nullptr)
        {
            next_->channelActive(channelContext);
        }
    }

    void DatagramChannelHandler::fireChannelRead(DatagramChannelContext& channelContext, DatagramPacket& message)
    {
        if (next_ != nullptr)
        {
            next_->channelRead(channelContext, message);
        }
    }
} // namespace nets