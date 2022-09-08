//
// Created by guang19
//

#ifndef NETS_NET_CHANNEL_HANDLER_PIPELINE_H
#define NETS_NET_CHANNEL_HANDLER_PIPELINE_H

#include <list>
#include <memory>

#include "nets/base/Copyable.h"
#include "nets/net/core/SocketChannelHandler.h"

namespace nets::net
{
    class ChannelHandlerPipeline : nets::base::Copyable
    {
    public:
        using ChannelRawPtr = ChannelContext::ChannelRawPtr;
        using SocketChannelHandlerRawPtr = SocketChannelHandler*;
        using SocketChannelHandlerPtr = ::std::shared_ptr<SocketChannelHandler>;
        using ChannelHandlerRawPtr = ChannelHandler*;
        using ChannelHandlerPtr = ::std::shared_ptr<ChannelHandler>;
        using ChannelHandlerList = ::std::list<ChannelHandlerPtr>;

    public:
        explicit ChannelHandlerPipeline(ChannelRawPtr channel);
        ~ChannelHandlerPipeline() = default;

        ChannelHandlerPipeline(const ChannelHandlerPipeline& other);
        ChannelHandlerPipeline(ChannelHandlerPipeline&& other) noexcept;
        ChannelHandlerPipeline& operator=(const ChannelHandlerPipeline& other);
        ChannelHandlerPipeline& operator=(ChannelHandlerPipeline&& other) noexcept;

    public:
        inline ChannelContext& context()
        {
            return channelContext_;
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
        void fireChannelConnect(const InetSockAddress& localAddress, const InetSockAddress& peerAddress);
        void fireChannelDisconnect();
        void fireChannelRead(ByteBuffer& message);
        void fireChannelWriteComplete();

    private:
        ChannelContext channelContext_ {nullptr};
        ChannelHandlerList channelHandlers_ {};
    };
} // namespace nets::net

#endif // NETS_NET_CHANNEL_HANDLER_PIPELINE_H