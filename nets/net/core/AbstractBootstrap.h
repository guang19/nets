//
// Created by guang19
//

#ifndef NETS_NET_ABSTRACT_BOOTSTRAP_H
#define NETS_NET_ABSTRACT_BOOTSTRAP_H

#include "nets/net/core/ChannelOption.h"
#include "nets/net/core/EventLoopGroup.h"

namespace nets::net
{
    template <class B>
    class AbstractBootstrap : nets::base::Noncopyable
    {
    public:
        using IntType = typename EventLoopGroup::IntType;
        using ChannelOptionList = ::std::vector<ChannelOption>;
        using EventLoopGroupPtr = ::std::unique_ptr<EventLoopGroup>;

    public:
        explicit AbstractBootstrap() : channelOptions_()
        {
            mainLoopGroup_ = ::std::make_unique<EventLoopGroup>(NumbOfMainEventLoops, MainEventLoopGroupName);
        }

        ~AbstractBootstrap() = default;

    public:
        B& option(const ChannelOption& channelOption, const ChannelOption::ValueType& value)
        {
            channelOptions_.emplace_back(channelOption.sockOpt(), value);
            return static_cast<B&>(*this);
        }

    protected:
        ChannelOptionList channelOptions_ {};
        EventLoopGroupPtr mainLoopGroup_ {nullptr};

    private:
        static constexpr IntType NumbOfMainEventLoops = 1;
        static constexpr char MainEventLoopGroupName[] = "MainLoopGroup";
    };
} // namespace nets::net

#endif // NETS_NET_ABSTRACT_BOOTSTRAP_H