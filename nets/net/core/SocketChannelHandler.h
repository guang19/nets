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

// @brief The SocketChannel event handler

#ifndef NETS_SOCKET_CHANNEL_HANDLER_H
#define NETS_SOCKET_CHANNEL_HANDLER_H

#include <memory>

#include "nets/base/Noncopyable.h"
#include "nets/net/core/InetSockAddress.h"

namespace nets
{
    class ByteBuffer;
    class SocketChannelContext;
    class SocketChannelHandler : Noncopyable, public ::std::enable_shared_from_this<SocketChannelHandler>
    {
    public:
        using StringType = ::std::string;
        using SocketChannelHandlerPtr = ::std::shared_ptr<SocketChannelHandler>;

    public:
        SocketChannelHandler();
        explicit SocketChannelHandler(const StringType& name);
        virtual ~SocketChannelHandler() = default;

    public:
        inline void setNext(const SocketChannelHandlerPtr& next)
        {
            next_ = next;
        }

        inline const SocketChannelHandlerPtr& next() const
        {
            return next_;
        }

        inline SocketChannelHandlerPtr& next()
        {
            return next_;
        }

        inline const StringType& name() const
        {
            return name_;
        }

        void addLast(const SocketChannelHandlerPtr& channelHandler);
        SocketChannelHandlerPtr findLastPrev();

        virtual void channelConnect(SocketChannelContext& channelContext, const InetSockAddress& localAddress,
                                    const InetSockAddress& peerAddress);
        virtual void channelDisconnect(SocketChannelContext& channelContext);
        virtual void channelRead(SocketChannelContext& channelContext, ByteBuffer& message);

        void fireChannelConnect(SocketChannelContext& channelContext, const InetSockAddress& localAddress,
                                const InetSockAddress& peerAddress);
        void fireChannelDisconnect(SocketChannelContext& channelContext);
        void fireChannelRead(SocketChannelContext& channelContext, ByteBuffer& message);

    private:
        StringType name_;
        SocketChannelHandlerPtr next_;
    };
} // namespace nets

#endif // NETS_SOCKET_CHANNEL_HANDLER_H