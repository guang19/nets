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

// @brief ChannelHandlerPipeline has the ability to chain call ChannelHandler

#include "nets/net/core/ChannelHandlerPipeline.h"

namespace nets
{
    ChannelHandlerPipeline::ChannelHandlerPipeline() : headChannelHandler_(nullptr)
    {

    }

    void ChannelHandlerPipeline::addFirst(const ChannelHandlerPtr& channelHandler)
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

    void ChannelHandlerPipeline::addLast(const ChannelHandlerPtr& channelHandler)
    {
        if (channelHandler != nullptr)
        {
            if (headChannelHandler_ != nullptr)
            {
                auto last = headChannelHandler_->findLast();
                last->setNext(channelHandler);
            }
            else
            {
                headChannelHandler_ = channelHandler;
            }
        }
    }

    ChannelHandlerPipeline::ChannelHandlerPtr ChannelHandlerPipeline::removeFirst()
    {
        if (headChannelHandler_ == nullptr)
        {
            return nullptr;
        }
        auto first = headChannelHandler_;
        unlinkHead();
        return first;
    }

    ChannelHandlerPipeline::ChannelHandlerPtr ChannelHandlerPipeline::removeLast()
    {
        if (headChannelHandler_ == nullptr)
        {
            return nullptr;
        }
        if (headChannelHandler_->next() == nullptr)
        {
            auto last = headChannelHandler_;
            unlinkHead();
            return last;
        }
        auto prev = headChannelHandler_->findLastPrev();
        auto last = prev->next();
        prev->setNext(nullptr);
        return last;
    }

    ChannelHandlerPipeline::ChannelHandlerPtr ChannelHandlerPipeline::remove(const StringType& name)
    {
        if (headChannelHandler_ == nullptr || name.empty())
        {
            return nullptr;
        }
        if (headChannelHandler_->name() == name)
        {
            auto first = headChannelHandler_;
            unlinkHead();
            return first;
        }
        auto prev = headChannelHandler_->findChannelHandlerPrev(name);
        if (prev != nullptr)
        {
            auto rm = prev->next();
            unlinkChannelHandler(prev, rm);
            return rm;
        }
        return nullptr;
    }

    bool ChannelHandlerPipeline::remove(const ChannelHandlerPtr& channelHandler)
    {
        if (headChannelHandler_ == nullptr || channelHandler == nullptr)
        {
            return false;
        }
        if (headChannelHandler_ == channelHandler)
        {
            unlinkHead();
            return true;
        }
        auto prev = headChannelHandler_->findChannelHandlerPrev(channelHandler);
        if (prev != nullptr)
        {
            auto rm = prev->next();
            unlinkChannelHandler(prev, rm);
            return true;
        }
        return false;
    }

    void ChannelHandlerPipeline::unlinkHead()
    {
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
    }

    void ChannelHandlerPipeline::unlinkChannelHandler(const ChannelHandlerPtr& prev, const ChannelHandlerPtr& channelHandler)
    {
        if (channelHandler->next() != nullptr)
        {
            prev->setNext(channelHandler->next());
            channelHandler->setNext(nullptr);
        }
        else
        {
            prev->setNext(nullptr);
        }
    }
} // namespace nets