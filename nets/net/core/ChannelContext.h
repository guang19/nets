//
// Created by guang19 on 2022/6/28.
//

#ifndef NETS_CHANNEL_CONTEXT_H
#define NETS_CHANNEL_CONTEXT_H

#include <memory>

#include "nets/base/Noncopyable.h"
#include "nets/net/core/ChannelHandlerPipeline.h"

namespace nets::net
{
    class Channel;

    class ChannelContext : nets::base::Noncopyable
    {
    public:
        using ChannelRawPtr = Channel*;
        using ChannelHandlerPipelineRawPtr = ChannelHandlerPipeline*;

    public:
        explicit ChannelContext(ChannelRawPtr channel);
        ~ChannelContext() = default;

    public:
        inline ChannelRawPtr channel()
        {
            return channel_;
        }

        inline ChannelHandlerPipelineRawPtr pipeline()
        {
            return &channelHandlerPipeline_;
        }

    private:
        ChannelRawPtr channel_ {nullptr};
        ChannelHandlerPipeline channelHandlerPipeline_ {};
    };
} // namespace nets::net

#endif // NETS_CHANNEL_CONTEXT_H
