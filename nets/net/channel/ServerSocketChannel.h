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

// @brief The encapsulation of tcp server operations

#ifndef NETS_SERVER_SOCKET_CHANNEL_H
#define NETS_SERVER_SOCKET_CHANNEL_H

#include <functional>

#include "nets/net/channel/SocketChannel.h"
#include "nets/net/core/InetSockAddress.h"

namespace nets
{
    class ServerSocketChannel : public Channel
    {
    public:
        using SocketChannelPtr = ::std::shared_ptr<SocketChannel>;
        using NextEventLoopFn = ::std::function<EventLoopRawPtr()>;
        using ChannelHandlerList = typename SocketChannelHandlerPipeline::SocketChannelHandlerList;
        using ChannelInitializationCallback = ::std::function<void(SocketChannel& channel)>;

    public:
        explicit ServerSocketChannel(EventLoopRawPtr eventLoop);
        ~ServerSocketChannel() override;

    public:
        FdType fd() const override;

    public:
        inline void setBacklog(::int32_t backlog) override
        {
            backlog_ = backlog;
        }

        inline void setNextEventLoopFn(const NextEventLoopFn& nextEventLoopFn)
        {
            nextEventLoopFn_ = nextEventLoopFn;
        }

        void setChannelOptions(const ChannelOptionList& channelOptions);
        void setChildOptions(const ChannelOptionList& childOptions);

        inline void setChildHandlers(const ChannelHandlerList& childHandlers)
        {
            childHandlers_ = childHandlers;
        }

        inline void setChildInitializationCallback(const ChannelInitializationCallback& childInitializationCallback)
        {
            childInitializationCallback_ = childInitializationCallback;
        }

        void bind(const InetSockAddress& localAddress);

    protected:
        void handleReadEvent() override;
        void handleErrorEvent() override;

    private:
        void initSocketChannel(SocketChannelPtr& socketChannel);
        void handleAcceptError(::int32_t errNum);

    private:
        FdType sockFd_;
        FdType idleFd_;
        ::int32_t backlog_;
        ChannelOptionList channelOptions_;
        NextEventLoopFn nextEventLoopFn_;
        ChannelOptionList childOptions_;
        ChannelHandlerList childHandlers_;
        ChannelInitializationCallback childInitializationCallback_;
    };
} // namespace nets

#endif // NETS_SERVER_SOCKET_CHANNEL_H