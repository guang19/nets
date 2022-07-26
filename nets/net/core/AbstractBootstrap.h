//
// Created by guang19 on 2022/7/26.
//

#ifndef NETS_ABSTRACT_BOOTSTRAP_H
#define NETS_ABSTRACT_BOOTSTRAP_H

#include <map>

#include "nets/net/core/ChannelOption.h"
#include "nets/net/core/EventLoopGroup.h"

namespace nets::net
{
    class AbstractBootstrap : nets::base::Noncopyable
    {
    public:
        using NType = typename EventLoopGroup::NType;
        using ChannelOptionSet = ::std::map<ChannelOption, ChannelOption::ValueType>;
        using EventLoopGroupPtr = ::std::unique_ptr<EventLoopGroup>;

    public:
        explicit AbstractBootstrap(NType numOfMainEventLoops);
        ~AbstractBootstrap() = default;

    public:
        template <class Self>
        Self& option(const ChannelOption& channelOption, ChannelOption::ValueType value)
        {
            channelOptions_[channelOption] = value;
            return static_cast<Self&>(*this);
        }

    protected:
        ChannelOptionSet channelOptions_ {};
        EventLoopGroupPtr mainLoopGroup_ {nullptr};
    };
} // namespace nets::net

#endif // NETS_ABSTRACT_BOOTSTRAP_H
