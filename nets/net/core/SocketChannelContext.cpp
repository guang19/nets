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

// @brief The holder of the SocketChannel

#include "nets/net/core/SocketChannelContext.h"

#include "nets/net/channel/SocketChannel.h"

namespace nets
{
    SocketChannelContext::SocketChannelContext(SocketChannelRawPtr channel) : channel_(channel) {}

    const InetSockAddress& SocketChannelContext::localAddress() const
    {
        return channel_->localAddress();
    }

    const InetSockAddress& SocketChannelContext::peerAddress() const
    {
        return channel_->peerAddress();
    }

    void SocketChannelContext::write(const void* message, SizeType length, WriteCompleteCallback writeCompleteCallback)
    {
        channel_->write(message, length, std::move(writeCompleteCallback));
    }

    void SocketChannelContext::write(const StringType& message, WriteCompleteCallback writeCompleteCallback)
    {
        channel_->write(message, std::move(writeCompleteCallback));
    }

    void SocketChannelContext::write(const ByteBuffer& message, WriteCompleteCallback writeCompleteCallback)
    {
        channel_->write(message, std::move(writeCompleteCallback));
    }

    bool SocketChannelContext::isActive() const
    {
        return channel_->isActive();
    }

    void SocketChannelContext::disconnect()
    {
        shutdown();
    }

    void SocketChannelContext::shutdown()
    {
        channel_->shutdown();
    }

    void SocketChannelContext::shutdownRead()
    {
        channel_->shutdownRead();
    }

    void SocketChannelContext::shutdownWrite()
    {
        channel_->shutdownWrite();
    }
} // namespace nets