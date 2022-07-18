//
// Created by guang19 on 2022/5/15.
//

#include "nets/net/core/Channel.h"

#include "nets/net/core/EventLoop.h"

namespace nets::net
{
    Channel::Channel(EventLoopRawPtr eventLoop)
        : events_(ENoneEvent), readyEvents_(ENoneEvent), isRegistered_(false), channelContext_(this), eventLoop_(eventLoop)
    {
    }

    bool Channel::registerTo()
    {
        return eventLoop_->registerChannel(shared_from_this());
    }

    bool Channel::modify()
    {
        return eventLoop_->modifyChannel(shared_from_this());
    }

    void Channel::deregister()
    {
        eventLoop_->deregisterChannel(shared_from_this());
    }

    void Channel::handleEvent()
    {
        if (readyEvents_ & EErrorEvent)
        {
            handleErrorEvent();
        }
        if (readyEvents_ & EReadEvent)
        {
            handleReadEvent();
        }
        if (readyEvents_ & EWriteEvent)
        {
            handleWriteEvent();
        }
    }

    void Channel::handleReadEvent()
    {
        LOGS_DEBUG << "Channel::handleReadEvent";
    }

    void Channel::handleWriteEvent()
    {
        LOGS_DEBUG << "Channel::handleWriteEvent";
    }

    void Channel::handleErrorEvent()
    {
        LOGS_DEBUG << "Channel::handleErrorEvent";
    }
} // namespace nets::net