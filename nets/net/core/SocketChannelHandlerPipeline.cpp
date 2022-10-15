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

#include "nets/net/core/SocketChannelHandlerPipeline.h"

#include "nets/net/core/ByteBuffer.h"
#include "nets/net/core/SocketChannelHandler.h"

namespace nets
{
    SocketChannelHandlerPipeline::SocketChannelHandlerPipeline(SocketChannelContextRawPtr channelContext)
        : ChannelHandlerPipeline(), channelContext_(channelContext)
    {
    }

    void SocketChannelHandlerPipeline::fireSocketChannelConnect(const InetSockAddress& localAddress,
                                                                const InetSockAddress& peerAddress)
    {
        if (headChannelHandler_ != nullptr)
        {
            ::std::dynamic_pointer_cast<SocketChannelHandler>(headChannelHandler_)
                ->channelConnect(*channelContext_, localAddress, peerAddress);
        }
    }

    void SocketChannelHandlerPipeline::fireSocketChannelDisconnect()
    {
        if (headChannelHandler_ != nullptr)
        {
            ::std::dynamic_pointer_cast<SocketChannelHandler>(headChannelHandler_)->channelDisconnect(*channelContext_);
        }
    }

    void SocketChannelHandlerPipeline::fireSocketChannelRead(ByteBuffer& message)
    {
        if (headChannelHandler_ != nullptr)
        {
            ::std::dynamic_pointer_cast<SocketChannelHandler>(headChannelHandler_)->channelRead(*channelContext_, message);
        }
    }
} // namespace nets