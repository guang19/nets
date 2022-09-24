//
// Created by guang19
//

#ifndef NETS_NET_SOCKET_CHANNEL_HANDLER_PIPELINE_H
#define NETS_NET_SOCKET_CHANNEL_HANDLER_PIPELINE_H

#include <list>

#include "nets/net/core/SocketChannelContext.h"
#include "nets/net/core/SocketChannelHandler.h"

namespace nets::net
{
    class SocketChannelHandlerPipeline : public nets::base::Copyable
    {
    public:
        using SocketChannelContextRawPtr = SocketChannelContext*;
        using SocketChannelContextPtr = ::std::unique_ptr<SocketChannelContext>;
        using SocketChannelHandlerRawPtr = SocketChannelHandler*;
        using SocketChannelHandlerPtr = ::std::shared_ptr<SocketChannelHandler>;
        using SocketChannelHandlerList = ::std::list<SocketChannelHandlerPtr>;

    public:
        explicit SocketChannelHandlerPipeline(SocketChannelContextRawPtr channelContext);
        ~SocketChannelHandlerPipeline() = default;

        SocketChannelHandlerPipeline(const SocketChannelHandlerPipeline& other);
        SocketChannelHandlerPipeline(SocketChannelHandlerPipeline&& other) noexcept;
        SocketChannelHandlerPipeline& operator=(const SocketChannelHandlerPipeline& other);
        SocketChannelHandlerPipeline& operator=(SocketChannelHandlerPipeline&& other) noexcept;

    public:
        inline SocketChannelContext& context()
        {
            return *channelContext_;
        }

        void addFirst(SocketChannelHandlerRawPtr channelHandler);
        void addFirst(const SocketChannelHandlerPtr& channelHandler);
        void addLast(SocketChannelHandlerRawPtr channelHandler);
        void addLast(const SocketChannelHandlerPtr& channelHandler);

    public:
        void fireSocketChannelConnect(const InetSockAddress& localAddress, const InetSockAddress& peerAddress);
        void fireSocketChannelDisconnect();
        void fireSocketChannelRead(ByteBuffer& message);

    private:
        SocketChannelContextPtr channelContext_ {};
        SocketChannelHandlerList channelHandlers_ {};
    };
} // namespace nets::net

#endif // NETS_NET_SOCKET_CHANNEL_HANDLER_PIPELINE_H