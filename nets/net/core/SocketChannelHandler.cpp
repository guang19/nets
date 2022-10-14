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

#include "nets/net/core/SocketChannelHandler.h"

#include "nets/base/log/Logger.h"
#include "nets/net/core/ByteBuffer.h"
#include "nets/net/core/SocketChannelContext.h"

namespace nets
{
    SocketChannelHandler::SocketChannelHandler() : name_(), next_(nullptr) {}

    SocketChannelHandler::SocketChannelHandler(const StringType& name) : name_(name), next_(nullptr) {}

    void SocketChannelHandler::addLast(const SocketChannelHandlerPtr& channelHandler)
    {
        if (next_ != nullptr)
        {
            auto temp = next_;
            while (temp->next_ != nullptr)
            {
                temp = temp->next_;
            }
            temp->next_ = channelHandler;
        }
        else
        {
            next_ = channelHandler;
        }
    }

    SocketChannelHandler::SocketChannelHandlerPtr SocketChannelHandler::findLastPrev()
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

    void SocketChannelHandler::channelConnect(SocketChannelContext& channelContext, const InetSockAddress& localAddress,
                                              const InetSockAddress& peerAddress)
    {
        NETS_SYSTEM_LOG_DEBUG << "SocketChannelHandler::channelConnect";
        fireChannelConnect(channelContext, localAddress, peerAddress);
    }

    void SocketChannelHandler::channelDisconnect(SocketChannelContext& channelContext)
    {
        NETS_SYSTEM_LOG_DEBUG << "SocketChannelHandler::channelDisconnect";
        fireChannelDisconnect(channelContext);
    }

    void SocketChannelHandler::channelRead(SocketChannelContext& channelContext, ByteBuffer& message)
    {
        NETS_SYSTEM_LOG_DEBUG << "SocketChannelHandler::channelRead";
        fireChannelRead(channelContext, message);
    }

    void SocketChannelHandler::fireChannelConnect(SocketChannelContext& channelContext, const InetSockAddress& localAddress,
                                                  const InetSockAddress& peerAddress)
    {
        if (next_ != nullptr)
        {
            next_->channelConnect(channelContext, localAddress, peerAddress);
        }
    }

    void SocketChannelHandler::fireChannelDisconnect(SocketChannelContext& channelContext)
    {
        if (next_ != nullptr)
        {
            next_->channelDisconnect(channelContext);
        }
    }

    void SocketChannelHandler::fireChannelRead(SocketChannelContext& channelContext, ByteBuffer& message)
    {
        if (next_ != nullptr)
        {
            next_->channelRead(channelContext, message);
        }
    }
} // namespace nets
