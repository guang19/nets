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
        : channelContext_(channelContext), headChannelHandler_(nullptr)
    {
    }

    void DatagramChannelHandlerPipeline::addFirst(const DatagramChannelHandlerPtr& channelHandler)
    {
        if (channelHandler != nullptr)
        {
            if (headChannelHandler_ != nullptr)
            {
                channelHandler->setNext(headChannelHandler_);
            }
            headChannelHandler_ = channelHandler;
        }
    }

    void DatagramChannelHandlerPipeline::addLast(const DatagramChannelHandlerPtr& channelHandler)
    {
        if (channelHandler != nullptr)
        {
            if (headChannelHandler_ != nullptr)
            {
                headChannelHandler_->addLast(channelHandler);
            }
            else
            {
                headChannelHandler_ = channelHandler;
            }
        }
    }

    DatagramChannelHandlerPipeline::DatagramChannelHandlerPtr DatagramChannelHandlerPipeline::removeFirst()
    {
        if (headChannelHandler_ == nullptr)
        {
            return nullptr;
        }
        auto ret = headChannelHandler_;
        auto next = headChannelHandler_->next();
        if (next != nullptr)
        {
            headChannelHandler_->setNext(nullptr);
            headChannelHandler_ = next;
        }
        else
        {
            headChannelHandler_ = nullptr;
        }
        return ret;
    }

    DatagramChannelHandlerPipeline::DatagramChannelHandlerPtr DatagramChannelHandlerPipeline::removeLast()
    {
        if (headChannelHandler_ == nullptr)
        {
            return nullptr;
        }
        if (headChannelHandler_->next() == nullptr)
        {
            auto ret = headChannelHandler_;
            headChannelHandler_ = nullptr;
            return ret;
        }
        DatagramChannelHandlerPtr ret {};
        auto prev = headChannelHandler_;
        auto temp = headChannelHandler_->next();
        for (;;)
        {
            if (temp->next() != nullptr)
            {
                prev = temp;
                temp = temp->next();
            }
            else
            {
                ret = temp;
                prev->setNext(nullptr);
                break;
            }
        }
        return ret;
    }

    DatagramChannelHandlerPipeline::DatagramChannelHandlerPtr DatagramChannelHandlerPipeline::remove(const StringType& name)
    {
        if (headChannelHandler_ == nullptr)
        {
            return nullptr;
        }
        if (headChannelHandler_->next() == nullptr)
        {
            if (headChannelHandler_->name() != name)
            {
                return nullptr;
            }
            else
            {
                auto ret = headChannelHandler_;
                headChannelHandler_ = nullptr;
                return ret;
            }
        }
        DatagramChannelHandlerPtr ret {};
        auto prev = headChannelHandler_;
        auto temp = headChannelHandler_->next();
        for (;;)
        {
            if (temp->name() != name)
            {
                if (temp->next() != nullptr)
                {
                    prev = temp;
                    temp = temp->next();
                }
                else
                {
                    break;
                }
            }
            else
            {
                ret = temp;
                if (temp->next() != nullptr)
                {
                    prev->setNext(temp->next());
                }
                else
                {
                    prev->setNext(nullptr);
                }
                break;
            }
        }
        return ret;
    }

    bool DatagramChannelHandlerPipeline::remove(const DatagramChannelHandlerPtr& channelHandler)
    {
        if (headChannelHandler_ == nullptr)
        {
            return false;
        }
        if (headChannelHandler_->next() == nullptr)
        {
            if (headChannelHandler_ == channelHandler)
            {
                headChannelHandler_ = nullptr;
                return true;
            }
            return false;
        }
        auto prev = headChannelHandler_;
        auto temp = headChannelHandler_->next();
        for (;;)
        {
            if (temp != channelHandler)
            {
                if (temp->next() != nullptr)
                {
                    prev = temp;
                    temp = temp->next();
                }
                else
                {
                    return false;
                }
            }
            else
            {
                if (temp->next() != nullptr)
                {
                    prev->setNext(temp->next());
                }
                else
                {
                    prev->setNext(nullptr);
                }
                return true;
            }
        }
    }

    void DatagramChannelHandlerPipeline::fireDatagramChannelActive()
    {
        if (headChannelHandler_ != nullptr)
        {
            headChannelHandler_->channelActive(*channelContext_);
        }
    }

    void DatagramChannelHandlerPipeline::fireDatagramChannelRead(DatagramPacket& message)
    {
        if (headChannelHandler_ != nullptr)
        {
            headChannelHandler_->channelRead(*channelContext_, message);
        }
    }
} // namespace nets