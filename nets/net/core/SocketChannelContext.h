//
// Created by guang19
//

#ifndef NETS_SOCKET_CHANNEL_CONTEXT_H
#define NETS_SOCKET_CHANNEL_CONTEXT_H

#include <functional>
#include <string>

#include "nets/base/Noncopyable.h"
#include "nets/net/core/InetSockAddress.h"

namespace nets
{
    class ByteBuffer;
    class SocketChannel;
    class SocketChannelContext : Noncopyable
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
        inline SocketChannel& channel()
        {
            return *channel_;
        }

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

    private:
        SocketChannelRawPtr channel_ {nullptr};
    };
} // namespace nets

#endif // NETS_SOCKET_CHANNEL_CONTEXT_H
