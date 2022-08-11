//
// Created by guang19 on 2022/5/19.
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
        using IntType = ByteBuffer::IntType;
        using StringType = ::std::string;

    public:
        explicit SocketChannel(FdType sockFd, const InetSockAddress& localAddress, const InetSockAddress& peerAddress,
                               EventLoopRawPtr eventLoop);
        ~SocketChannel() override;

    public:
        FdType fd() const override;
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

        void write(const StringType& message);
        void write(const void* message, IntType len);
        void write(const ByteBuffer& message);

        // shutdown RD and WR
        void shutdown();
        void shutdownRead();
        void shutdownWrite();

    private:
        void handleReadError(int32_t errNum);
        void shutdown(int32_t how);

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
