//
// Created by guang19
//

#ifndef NETS_NET_SOCKET_CHANNEL_H
#define NETS_NET_SOCKET_CHANNEL_H

#include "nets/net/core/ByteBuffer.h"
#include "nets/net/core/Channel.h"
#include "nets/net/core/ChannelHandlerPipeline.h"
#include <queue>

namespace nets::net
{
    class ByteBuffer;
    class SocketChannel : public Channel
    {
    public:
        using SizeType = ByteBuffer::SizeType;
        using StringType = ::std::string;
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
        inline ChannelHandlerPipeline& pipeline()
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
        SSizeType writev(const IoVecList& iovecs, ::int32_t count) const;
        void removeSentBuffer(SSizeType writtenBytes);
        void handleReadError(::int32_t errNum);
        void handleWriteError(::int32_t errNum);
        void channelInActive();
        void shutdown(::int32_t how);

    private:
        FdType sockFd_ {socket::InvalidFd};
        InetSockAddress localAddress_ {};
        InetSockAddress peerAddress_ {};
        using BufferList = ::std::vector<ByteBuffer>;
        BufferList writeBuffer_ {};

        enum ChannelState
        {
            INACTIVE,
            ACTIVE,
            HALF_CLOSE,
        };
        ChannelState state_ {ChannelState::INACTIVE};
        ChannelHandlerPipeline channelHandlerPipeline_ {nullptr};
        WriteCompleteCallbackQueue writeCompleteCallbacks_ {};
    };
} // namespace nets::net

#endif // NETS_NET_SOCKET_CHANNEL_H