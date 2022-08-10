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
        inline FdType fd() const override
        {
            return sockFd_;
        }

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

    public:
        void channelActive();
        void setChannelOptions(const ChannelOptionList& channelOptions);

        void connect();
        void write(const StringType& message);
        void write(const void* message, IntType len);
        void write(const ByteBuffer& message);

    public:
        void handleReadEvent() override;
        void handleWriteEvent() override;
        void handleErrorEvent() override;

    private:
        void handleReadError(int32_t errNum);
        void handleConnectError(int32_t errNum);
        void reconnect();

    private:
        FdType sockFd_ {socket::InvalidFd};
        InetSockAddress localAddress_ {};
        InetSockAddress peerAddress_ {};
        using BufferList = ::std::vector<ByteBuffer>;
        BufferList sendBuffer_ {};

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
