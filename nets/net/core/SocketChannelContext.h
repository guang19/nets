//
// Created by guang19
//

#ifndef NETS_NET_SOCKET_CHANNEL_CONTEXT_H
#define NETS_NET_SOCKET_CHANNEL_CONTEXT_H

#include <functional>

#include "nets/net/core/ByteBuffer.h"
#include "nets/net/core/ChannelContext.h"

namespace nets::net
{
    class SocketChannelContext : public ChannelContext
    {
    public:
        using SocketChannelRawPtr = SocketChannel*;
        using SizeType = ::size_t;
        using StringType = ::std::string;
        using WriteCompleteCallback = ::std::function<void(SocketChannelContext&)>;

    public:
        explicit SocketChannelContext(SocketChannelRawPtr channel);
        ~SocketChannelContext() = default;

    public:
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

    public:
        inline SocketChannel& channel()
        {
            return *channel_;
        }

    private:
        SocketChannelRawPtr channel_ {nullptr};
    };
} // namespace nets::net

#endif // NETS_NET_SOCKET_CHANNEL_CONTEXT_H
