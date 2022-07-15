//
// Created by guang19 on 2022/5/16.
//

#include "nets/net/poller/Poller.h"

#include "nets/net/poller/EpollPoller.h"

namespace nets::net
{
    PollerFactory::PollerPtr PollerFactory::getPoller(EventLoopRawPtr eventLoop)
    {
        return ::std::make_unique<EpollPoller>(eventLoop);
    }
}; // namespace nets::net