//
// Created by guang19 on 2022/7/26.
//

#ifndef NETS_ABSTRACT_BOOTSTRAP_H
#define NETS_ABSTRACT_BOOTSTRAP_H

#include "nets/net/core/ChannelOption.h"
#include "nets/net/core/EventLoopGroup.h"

namespace nets::net
{
    template <class SubType>
    class AbstractBootstrap : nets::base::Noncopyable
    {
    public:
        using IntType = typename EventLoopGroup::IntType;
        using ChannelOptionList = ::std::vector<ChannelOption>;
        using EventLoopGroupPtr = ::std::unique_ptr<EventLoopGroup>;

    public:
        explicit AbstractBootstrap(IntType numOfMainEventLoops) : channelOptions_()
        {
            numOfMainEventLoops = numOfMainEventLoops <= 0 ? DefaultNumbOfMainEventLoops : numOfMainEventLoops;
            mainLoopGroup_ = ::std::make_unique<EventLoopGroup>(numOfMainEventLoops, MainEventLoopGroupName);
        }
        ~AbstractBootstrap() = default;

    public:
        SubType& option(const ChannelOption& channelOption, const ChannelOption::ValueType& value)
        {
            channelOptions_.emplace_back(channelOption.sockOpt(), value);
            return static_cast<SubType&>(*this);
        }

    protected:
        ChannelOptionList channelOptions_ {};
        EventLoopGroupPtr mainLoopGroup_ {nullptr};

    private:
        static constexpr char MainEventLoopGroupName[] = "MainLoopGroup";
        static constexpr IntType DefaultNumbOfMainEventLoops = 1;
    };
} // namespace nets::net

#endif // NETS_ABSTRACT_BOOTSTRAP_H
