//
// Created by guang19 on 2022/6/28.
//

#ifndef NETS_NET_CHANNEL_CONTEXT_H
#define NETS_NET_CHANNEL_CONTEXT_H

#include "nets/base/Noncopyable.h"

namespace nets::net
{
    class SocketChannel;
    class ChannelContext : nets::base::Noncopyable
    {
    public:
        using ChannelRawPtr = SocketChannel*;

    public:
        explicit ChannelContext(ChannelRawPtr channel);
        ~ChannelContext() = default;

    public:
        bool isActive() const;
        // shutdown RD and WR
        void shutdown();
        void shutdownRead();
        void shutdownWrite();

    public:
        inline SocketChannel& channel()
        {
            return *channel_;
        }

    private:
        ChannelRawPtr channel_ {nullptr};
    };
} // namespace nets::net

#endif // NETS_NET_CHANNEL_CONTEXT_H
