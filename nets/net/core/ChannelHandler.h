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

#ifndef NETS_CHANNEL_HANDLER_H
#define NETS_CHANNEL_HANDLER_H

#include <memory>
#include <string>

#include "nets/base/Noncopyable.h"
#include "nets/base/Types.h"

namespace nets
{
    class ChannelHandler : Noncopyable, public ::std::enable_shared_from_this<ChannelHandler>
    {
    public:
        using ChannelHandlerPtr = ::std::shared_ptr<ChannelHandler>;

    public:
        ChannelHandler();
        explicit ChannelHandler(const StringType& name);
        virtual ~ChannelHandler() = default;

    public:
        inline void setNext(const ChannelHandlerPtr& next)
        {
            next_ = next;
        }

        inline const ChannelHandlerPtr& next() const
        {
            return next_;
        }

        inline const StringType& name() const
        {
            return name_;
        }

        ChannelHandlerPtr findLast();
        ChannelHandlerPtr findLastPrev();
        ChannelHandlerPtr findChannelHandlerPrev(const StringType& name);
        ChannelHandlerPtr findChannelHandlerPrev(const ChannelHandlerPtr& channelHandler);

    protected:
        StringType name_;
        ChannelHandlerPtr next_;
    };
} // namespace nets

#endif // NETS_CHANNEL_HANDLER_H