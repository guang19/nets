//
// Created by guang19 on 2022/7/26.
//

#include "nets/net/core/AbstractBootstrap.h"

namespace nets::net
{
    namespace
    {
        constexpr char MainEventLoopGroupName[] = "MainLoopGroup";
        constexpr AbstractBootstrap::NType DefaultNumbOfMainEventLoops = 1;
    }

    AbstractBootstrap::AbstractBootstrap(NType numOfMainEventLoops) : channelOptions_()
    {
        numOfMainEventLoops = numOfMainEventLoops <= 0 ? DefaultNumbOfMainEventLoops : numOfMainEventLoops;
        mainLoopGroup_ = ::std::make_unique<EventLoopGroup>(numOfMainEventLoops, MainEventLoopGroupName);
    }
}