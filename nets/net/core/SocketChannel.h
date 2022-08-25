//
// Created by guang19
//

#ifndef NETS_NET_SOCKET_CHANNEL_H
#define NETS_NET_SOCKET_CHANNEL_H

#include "nets/net/core/ByteBuffer.h"
#include "nets/net/core/Channel.h"
#include "nets/net/core/ChannelHandlerPipeline.h"

namespace nets::net
{
    class ByteBuffer;

    class SocketChannel : public Channel
    {
    public:
        using SizeType = ByteBuffer::SizeType;
        using StringType = ::std::string;
        using IoVecList = ::std::vector<IoVec>;

    public:
        explicit SocketChannel(FdType sockFd, const InetSockAddress& localAddress, const InetSockAddress& peerAddress,
                               EventLoopRawPtr eventLoop);
        ~SocketChannel() override;

    public:
        FdType fd() const override;

    protected:
        void handleReadEvent() override;
        void handleWriteEvent() override;
        void handleErrorEvent() override;

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

        void write(const void* message, SizeType length);
        void write(const StringType& message);
        void write(const ByteBuffer& message);

        // shutdown both
        void shutdown();
        // shutdown read
        void shutdownRead();
        // shutdown write
        void shutdownWrite();


    private:
        SSizeType doRead(ByteBuffer& byteBuffer);
        void doWrite(const void* data, SizeType length);
        void doWriteDirectly(const void* data, SizeType length);
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
    };
} // namespace nets::net

#endif // NETS_NET_SOCKET_CHANNEL_H