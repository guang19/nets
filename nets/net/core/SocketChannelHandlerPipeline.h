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

// @brief Manage SocketChannelHandler owned by SocketChannel

#ifndef NETS_SOCKET_CHANNEL_HANDLER_PIPELINE_H
#define NETS_SOCKET_CHANNEL_HANDLER_PIPELINE_H

#include "nets/net/core/ChannelHandlerPipeline.h"
#include "nets/net/core/SocketChannelContext.h"

namespace nets
{
    class SocketChannelHandlerPipeline : public ChannelHandlerPipeline
    {
    private:
        using SocketChannelContextRawPtr = SocketChannelContext*;
        using SocketChannelContextPtr = ::std::unique_ptr<SocketChannelContext>;

    public:
        explicit SocketChannelHandlerPipeline(SocketChannelContextRawPtr channelContext);
        ~SocketChannelHandlerPipeline() override = default;

    public:
        inline SocketChannelContext& context()
        {
            return *channelContext_;
        }

        void fireSocketChannelConnect(const InetSockAddress& localAddress, const InetSockAddress& peerAddress);
        void fireSocketChannelDisconnect();
        void fireSocketChannelRead(ByteBuffer& message);

    private:
        SocketChannelContextPtr channelContext_;
    };
} // namespace nets

#endif // NETS_SOCKET_CHANNEL_HANDLER_PIPELINE_H