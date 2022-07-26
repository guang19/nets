//
// Created by guang19 on 2022/5/18.
//

#ifndef NETS_NET_SERVER_SOCKET_CHANNEL_H
#define NETS_NET_SERVER_SOCKET_CHANNEL_H

#include <functional>

#include "nets/net/core/Channel.h"
#include "nets/net/core/InetSockAddress.h"
#include "nets/net/core/SocketChannel.h"

namespace nets::net
{
    class ServerSocketChannel : public Channel
    {
    public:
        using SocketChannelPtr = ::std::shared_ptr<SocketChannel>;
        using ChannelInitializationCallback = ::std::function<void(SocketChannelPtr& channel)>;
        using NextEventLoopFn = ::std::function<EventLoopRawPtr()>;
        using ChannelHandlerPtr = typename ChannelHandlerPipeline::ChannelHandlerPtr;
        using ChannelHandlerList = typename ChannelHandlerPipeline::ChannelHandlerList;

    public:
        explicit ServerSocketChannel(EventLoopRawPtr eventLoop);
        ~ServerSocketChannel() override;

    public:
        inline FdType fd() const override
        {
            return sockFd_;
        }

        inline void setNextEventLoopFn(const NextEventLoopFn& nextEventLoopFn)
        {
            nextEventLoopFn_ = nextEventLoopFn;
        }

        void setChannelOptions(const ChannelOptionList& channelOptions) override;
        void setChannelOptions();
        void setChildOptions(const ChannelOptionList& childOptions);

        inline void setChildHandlers(const ChannelHandlerList& childHandlers)
        {
            childHandlers_ = childHandlers;
        }

        inline void setChildInitializationCallback(const ChannelInitializationCallback& childInitializationCallback)
        {
            childInitializationCallback_ = childInitializationCallback;
        }

        void bind(const InetSockAddress& sockAddress);

    public:
        void handleReadEvent() override;
        void handleErrorEvent() override;

    private:
        FdType sockFd_ {socket::InvalidFd};
        FdType idleFd_ {socket::InvalidFd};
        int32_t backlog_ {0};
        ChannelOptionList channelOptions_ {};
        NextEventLoopFn nextEventLoopFn_ {};
        ChannelOptionList childOptions_ {};
        ChannelHandlerList childHandlers_ {};
        ChannelInitializationCallback childInitializationCallback_ {};
    };
} // namespace nets::net

#endif // NETS_NET_SERVER_SOCKET_CHANNEL_H