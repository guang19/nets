//
// Created by guang19
//

#ifndef NETS_NET_CHANNEL_HANDLER_PIPELINE_H
#define NETS_NET_CHANNEL_HANDLER_PIPELINE_H

#include <list>
#include <memory>

#include "nets/net/core/ChannelContext.h"
#include "nets/net/core/DatagramChannelHandler.h"
#include "nets/net/core/SocketChannelHandler.h"

namespace nets::net
{
    class ChannelHandlerPipeline : nets::base::Copyable
    {
    public:
        using ChannelContextRawPtr = ChannelContext*;
        using ChannelContextPtr = ::std::unique_ptr<ChannelContext>;
        using SocketChannelHandlerRawPtr = SocketChannelHandler*;
        using SocketChannelHandlerPtr = ::std::shared_ptr<SocketChannelHandler>;
        using ChannelHandlerRawPtr = ChannelHandler*;
        using ChannelHandlerPtr = ::std::shared_ptr<ChannelHandler>;
        using ChannelHandlerList = ::std::list<ChannelHandlerPtr>;

    public:
        explicit ChannelHandlerPipeline(ChannelContextRawPtr channelContext);
        ~ChannelHandlerPipeline() = default;

        ChannelHandlerPipeline(const ChannelHandlerPipeline& other);
        ChannelHandlerPipeline(ChannelHandlerPipeline&& other) noexcept;
        ChannelHandlerPipeline& operator=(const ChannelHandlerPipeline& other);
        ChannelHandlerPipeline& operator=(ChannelHandlerPipeline&& other) noexcept;

    public:
        inline ChannelContext& context()
        {
            return *channelContext_;
        }

        void addFirst(ChannelHandlerRawPtr channelHandler);
        void addFirst(const ChannelHandlerPtr& channelHandler);
        void addLast(ChannelHandlerRawPtr channelHandler);
        void addLast(const ChannelHandlerPtr& channelHandler);

        void addFirst(SocketChannelHandlerRawPtr channelHandler);
        void addFirst(const SocketChannelHandlerPtr& channelHandler);
        void addLast(SocketChannelHandlerRawPtr channelHandler);
        void addLast(const SocketChannelHandlerPtr& channelHandler);

    public:
        void fireSocketChannelConnect(const InetSockAddress& localAddress, const InetSockAddress& peerAddress);
        void fireSocketChannelDisconnect();
        void fireSocketChannelRead(ByteBuffer& message);

        void fireDatagramChannelActive();
        void fireDatagramChannelRead(DatagramPacket& message);

    private:
        ChannelContextPtr channelContext_ {};
        ChannelHandlerList channelHandlers_ {};
    };
} // namespace nets::net

#endif // NETS_NET_CHANNEL_HANDLER_PIPELINE_H