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

// @brief The encapsulation of tcp socket operations

#ifndef NETS_SOCKET_CHANNEL_H
#define NETS_SOCKET_CHANNEL_H

#include <queue>

#include "nets/net/channel/Channel.h"
#include "nets/net/core/ByteBuffer.h"
#include "nets/net/core/SocketChannelHandlerPipeline.h"

namespace nets
{
    class SocketChannel : public Channel
    {
    private:
        using IoVecList = ::std::vector<IoVec>;
        using WriteCompleteCallback = SocketChannelContext::WriteCompleteCallback;
        using WriteCompleteCallbackQueue = ::std::queue<WriteCompleteCallback>;

    public:
        explicit SocketChannel(FdType sockFd, const InetSockAddress& localAddress, const InetSockAddress& peerAddress,
                               EventLoopRawPtr eventLoop);
        ~SocketChannel() override;

    public:
        FdType fd() const override;

    public:
        inline SocketChannelHandlerPipeline& pipeline()
        {
            return channelHandlerPipeline_;
        }

        inline const InetSockAddress& localAddress() const
        {
            return localAddress_;
        }

        inline const InetSockAddress& peerAddress() const
        {
            return peerAddress_;
        }

        inline bool isActive() const
        {
            return state_ == ChannelState::ACTIVE;
        }

        void setChannelOptions(const ChannelOptionList& channelOptions);
        void channelActive();

        void write(const void* message, SizeType length, WriteCompleteCallback writeCompleteCallback);
        void write(const StringType& message, WriteCompleteCallback writeCompleteCallback);
        void write(const ByteBuffer& message, WriteCompleteCallback writeCompleteCallback);

        // shutdown both
        void shutdown();
        // shutdown read
        void shutdownRead();
        // shutdown write
        void shutdownWrite();

    protected:
        void handleReadEvent() override;
        void handleWriteEvent() override;
        void handleErrorEvent() override;

    private:
        SSizeType doRead(ByteBuffer& byteBuffer);
        void doWrite(const void* data, SizeType length, WriteCompleteCallback writeCompleteCallback);
        void doWriteDirectly(const void* data, SizeType length, WriteCompleteCallback writeCompleteCallback);
        void appendBuffer(const void* data, SizeType length);
        bool writeBufferLastCanAppend(SizeType length);
        SSizeType writev(const IoVecList& iovecs, Int32Type count) const;
        void removeSentBuffer(SSizeType writtenBytes);
        void handleReadError(Int32Type errNum);
        void handleWriteError(Int32Type errNum);
        void channelInActive();
        void shutdown(Int32Type how);

    private:
        FdType sockFd_;
        InetSockAddress localAddress_;
        InetSockAddress peerAddress_;
        using BufferList = ::std::vector<ByteBuffer>;
        BufferList writeBuffer_;

        enum ChannelState
        {
            INACTIVE,
            ACTIVE,
            HALF_CLOSE,
        };
        ChannelState state_;
        SocketChannelHandlerPipeline channelHandlerPipeline_;
        WriteCompleteCallbackQueue writeCompleteCallbacks_;
    };
} // namespace nets

#endif // NETS_SOCKET_CHANNEL_H