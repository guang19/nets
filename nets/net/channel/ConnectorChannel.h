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

// @brief The encapsulation of tcp client operations

#ifndef NETS_CONNECTOR_CHANNEL_H
#define NETS_CONNECTOR_CHANNEL_H

#include <functional>

#include "nets/net/channel/SocketChannel.h"

namespace nets
{
    class ConnectorChannel : public Channel
    {
    public:
        using TimeType = ::time_t;
        using SocketChannelPtr = ::std::shared_ptr<SocketChannel>;
        using ChannelHandlerList = typename SocketChannelHandlerPipeline::SocketChannelHandlerList;
        using ChannelInitializationCallback = ::std::function<void(SocketChannel& channel)>;

    public:
        explicit ConnectorChannel(EventLoopRawPtr eventLoop);
        ~ConnectorChannel() override;

    public:
        FdType fd() const override;

    protected:
        void handleWriteEvent() override;
        void handleErrorEvent() override;

    public:
        inline void setRetry(bool retry)
        {
            retry_ = retry;
        }

        inline void setRetryInterval(TimeType retryInterval)
        {
            retryInterval_ = retryInterval;
        }

        inline void setChannelOptions(const ChannelOptionList& channelOptions)
        {
            channelOptions_ = channelOptions;
        }

        inline void setChannelHandlers(const ChannelHandlerList& channelHandlers)
        {
            channelHandlers_ = channelHandlers;
        }

        inline void setChannelInitializationCallback(const ChannelInitializationCallback& channelInitializationCallback)
        {
            channelInitializationCallback_ = channelInitializationCallback;
        }

        void connect(const InetSockAddress& serverAddress);

    private:
        void initSocketChannel(SocketChannelPtr& socketChannel);
        void newSocketChannel();
        void handleConnectError(::int32_t errNum);
        void waitConnect();
        void reconnect();

    private:
        FdType sockFd_;
        InetSockAddress localAddress_;
        InetSockAddress peerAddress_;

        enum ConnectionState
        {
            CONNECTED,
            CONNECTING,
            DISCONNECTED
        };
        ConnectionState state_;
        bool retry_;
        TimeType retryInterval_;
        ChannelOptionList channelOptions_;
        ChannelHandlerList channelHandlers_;
        ChannelInitializationCallback channelInitializationCallback_;
    };
} // namespace nets

#endif // NETS_CONNECTOR_CHANNEL_H