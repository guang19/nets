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

#ifndef NETS_CHANNEL_HANDLER_PIPELINE_H
#define NETS_CHANNEL_HANDLER_PIPELINE_H

#include "nets/net/core/ChannelHandler.h"

namespace nets
{
    class ChannelHandlerPipeline : Noncopyable
    {
    public:
        using StringType = ChannelHandler::StringType;
        using ChannelHandlerPtr = ChannelHandler::ChannelHandlerPtr;

    public:
        ChannelHandlerPipeline();
        virtual ~ChannelHandlerPipeline() = default;

    public:
        void addFirst(const ChannelHandlerPtr& channelHandler);
        void addLast(const ChannelHandlerPtr& channelHandler);

        ChannelHandlerPtr removeFirst();
        ChannelHandlerPtr removeLast();
        ChannelHandlerPtr remove(const StringType& name);
        bool remove(const ChannelHandlerPtr& channelHandler);

    private:
        void unlinkHead();
        void unlinkChannelHandler(const ChannelHandlerPtr& prev, const ChannelHandlerPtr& channelHandler);

    protected:
        ChannelHandlerPtr headChannelHandler_;
    };
} // namespace nets

#endif // NETS_CHANNEL_HANDLER_PIPELINE_H