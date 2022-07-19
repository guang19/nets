//
// Created by guang19 on 2022/5/18.
//

#ifndef NETS_NET_SERVER_SOCKET_CHANNEL_H
#define NETS_NET_SERVER_SOCKET_CHANNEL_H

#include <functional>

#include "nets/net/core/Channel.h"
#include "nets/net/core/InetSockAddress.h"

namespace nets::net
{
    class ServerSocketChannel : public Channel
    {
    public:
        using ChannelPtr = ::std::shared_ptr<Channel>;
        using ChannelInitializationCallback = ::std::function<void(ChannelPtr channel)>;
        using ChannelHandlerPtr = typename ChannelHandlerPipeline::ChannelHandlerPtr;
        using ChannelHandlerList = typename ChannelHandlerPipeline::ChannelHandlerList;

    public:
        explicit ServerSocketChannel(EventLoopRawPtr eventLoop);
        ~ServerSocketChannel() override;

    public:
        inline void addChannelHandler(ChannelHandlerPtr channelHandler)
        {
            channelHandlers_.push_back(channelHandler);
        }

        inline void setChannelInitializationCallback(const ChannelInitializationCallback& channelInitializationCallback)
        {
            channelInitializationCallbacks_ = channelInitializationCallback;
        }

        inline FdType fd() const override
        {
            return sockFd_;
        }

        void bind(const InetSockAddress& sockAddress);

    public:
        void handleReadEvent() override;
        void handleErrorEvent() override;

    private:
        FdType sockFd_ {socket::InvalidFd};
        FdType idleFd_ {socket::InvalidFd};
        ChannelHandlerList channelHandlers_ {};
        ChannelInitializationCallback channelInitializationCallbacks_ {};
    };
} // namespace nets::net

#endif // NETS_NET_SERVER_SOCKET_CHANNEL_H