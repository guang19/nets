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

#ifndef NETS_DATAGRAM_CHANNEL_HANDLER_PIPELINE_H
#define NETS_DATAGRAM_CHANNEL_HANDLER_PIPELINE_H

#include <list>

#include "nets/net/core/DatagramChannelContext.h"
#include "nets/net/core/DatagramChannelHandler.h"

namespace nets
{
    class DatagramChannelHandlerPipeline : public Copyable
    {
    public:
        using DatagramChannelContextRawPtr = DatagramChannelContext*;
        using DatagramChannelContextPtr = ::std::unique_ptr<DatagramChannelContext>;
        using DatagramChannelHandlerRawPtr = DatagramChannelHandler*;
        using DatagramChannelHandlerPtr = ::std::shared_ptr<DatagramChannelHandler>;
        using DatagramChannelHandlerList = ::std::list<DatagramChannelHandlerPtr>;

    public:
        explicit DatagramChannelHandlerPipeline(DatagramChannelContextRawPtr channelContext);
        ~DatagramChannelHandlerPipeline() = default;

        DatagramChannelHandlerPipeline(const DatagramChannelHandlerPipeline& other);
        DatagramChannelHandlerPipeline(DatagramChannelHandlerPipeline&& other) noexcept;
        DatagramChannelHandlerPipeline& operator=(const DatagramChannelHandlerPipeline& other);
        DatagramChannelHandlerPipeline& operator=(DatagramChannelHandlerPipeline&& other) noexcept;

    public:
        inline DatagramChannelContext& context()
        {
            return *channelContext_;
        }

        void addFirst(DatagramChannelHandlerRawPtr channelHandler);
        void addFirst(const DatagramChannelHandlerPtr& channelHandler);
        void addLast(DatagramChannelHandlerRawPtr channelHandler);
        void addLast(const DatagramChannelHandlerPtr& channelHandler);

    public:
        void fireDatagramChannelActive();
        void fireDatagramChannelRead(DatagramPacket& message);

    private:
        DatagramChannelContextPtr channelContext_;
        DatagramChannelHandlerList channelHandlers_;
    };
} // namespace nets

#endif // NETS_DATAGRAM_CHANNEL_HANDLER_PIPELINE_H