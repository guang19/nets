//
// Created by guang19
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
        using NextEventLoopFn = ::std::function<EventLoopRawPtr()>;
        using ChannelHandlerList = typename SocketChannelHandlerPipeline::SocketChannelHandlerList;
        using ChannelInitializationCallback = ::std::function<void(SocketChannel& channel)>;

    public:
        explicit ServerSocketChannel(EventLoopRawPtr eventLoop);
        ~ServerSocketChannel() override;

    public:
        FdType fd() const override;

    public:
        inline void setBacklog(::int32_t backlog) override
        {
            backlog_ = backlog;
        }

        inline void setNextEventLoopFn(const NextEventLoopFn& nextEventLoopFn)
        {
            nextEventLoopFn_ = nextEventLoopFn;
        }

        inline void setChannelOptions(const ChannelOptionList& channelOptions)
        {
            channelOptions_.insert(channelOptions_.end(), channelOptions.begin(), channelOptions.end());
        }

        inline void setChildOptions(const ChannelOptionList& childOptions)
        {
            childOptions_.insert(childOptions_.end(), childOptions.begin(), childOptions.end());
        }

        inline void setChildHandlers(const ChannelHandlerList& childHandlers)
        {
            childHandlers_ = childHandlers;
        }

        inline void setChildInitializationCallback(const ChannelInitializationCallback& childInitializationCallback)
        {
            childInitializationCallback_ = childInitializationCallback;
        }

        void bind(const InetSockAddress& localAddress);

    protected:
        void handleReadEvent() override;
        void handleErrorEvent() override;

    private:
        void initSocketChannel(SocketChannelPtr& socketChannel);
        void handleAcceptError(::int32_t errNum);

    private:
        FdType sockFd_ {socket::InvalidFd};
        FdType idleFd_ {socket::InvalidFd};
        ::int32_t backlog_ {0};
        ChannelOptionList channelOptions_ {};
        NextEventLoopFn nextEventLoopFn_ {};
        ChannelOptionList childOptions_ {};
        ChannelHandlerList childHandlers_ {};
        ChannelInitializationCallback childInitializationCallback_ {};
    };
} // namespace nets::net

#endif // NETS_NET_SERVER_SOCKET_CHANNEL_H