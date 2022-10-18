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

#ifndef NETS_SOCKET_CHANNEL_CONTEXT_H
#define NETS_SOCKET_CHANNEL_CONTEXT_H

#include <functional>
#include <string>

#include "nets/base/Noncopyable.h"
#include "nets/net/core/InetSockAddress.h"

namespace nets
{
    class ByteBuffer;
    class SocketChannel;
    class SocketChannelContext : Noncopyable
    {
    public:
        using SocketChannelRawPtr = SocketChannel*;
        using WriteCompleteCallback = ::std::function<void(SocketChannelContext&)>;

    public:
        explicit SocketChannelContext(SocketChannelRawPtr channel);
        ~SocketChannelContext() = default;

    public:
        inline SocketChannel& channel()
        {
            return *channel_;
        }

        const InetSockAddress& localAddress() const;
        const InetSockAddress& peerAddress() const;

        void write(const void* message, SizeType length, WriteCompleteCallback writeCompleteCallback = nullptr);
        void write(const StringType& message, WriteCompleteCallback writeCompleteCallback = nullptr);
        void write(const ByteBuffer& message, WriteCompleteCallback writeCompleteCallback = nullptr);

        bool isActive() const;
        void disconnect();
        // shutdown both
        void shutdown();
        // shutdown read
        void shutdownRead();
        // shutdown write
        void shutdownWrite();

    private:
        SocketChannelRawPtr channel_;
    };
} // namespace nets

#endif // NETS_SOCKET_CHANNEL_CONTEXT_H
