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

// @brief ChannelHandler is a singly linked list structure

#include "nets/net/core/ChannelHandler.h"

namespace nets
{
    ChannelHandler::ChannelHandler() : name_(), next_(nullptr) {}

    ChannelHandler::ChannelHandler(const StringType& name) : name_(name), next_(nullptr) {}

    ChannelHandler::ChannelHandlerPtr ChannelHandler::findLast()
    {
        // if next is nullptr,the current node is the only node
        if (next_ == nullptr)
        {
            return shared_from_this();
        }
        auto temp = next_;
        while (temp->next_ != nullptr)
        {
            temp = temp->next_;
        }
        return temp;
    }

    ChannelHandler::ChannelHandlerPtr ChannelHandler::findLastPrev()
    {
        auto prev = shared_from_this();
        auto temp = next_;
        while (temp->next_ != nullptr)
        {
            prev = temp;
            temp = temp->next_;
        }
        return prev;
    }

    ChannelHandler::ChannelHandlerPtr ChannelHandler::findChannelHandlerPrev(const StringType& name)
    {
        auto prev = shared_from_this();
        auto temp = next_;
        while (temp != nullptr)
        {
            if (temp->name_ == name)
            {
                return prev;
            }
            else
            {
                prev = temp;
                temp = temp->next_;
            }
        }
        return nullptr;
    }

    ChannelHandler::ChannelHandlerPtr ChannelHandler::findChannelHandlerPrev(const ChannelHandlerPtr& channelHandler)
    {
        auto prev = shared_from_this();
        auto temp = next_;
        while (temp != nullptr)
        {
            if (temp == channelHandler)
            {
                return prev;
            }
            else
            {
                prev = temp;
                temp = temp->next_;
            }
        }
        return nullptr;
    }
} // namespace nets