//
// Created by guang19 on 2022/5/15.
//

#include "nets/net/core/Channel.h"

#include "nets/net/core/EventLoop.h"

namespace nets::net
{
    Channel::Channel(EventLoopRawPtr eventLoop)
        : events_(ENoneEvent), readyEvents_(ENoneEvent), isRegistered_(false), eventLoop_(eventLoop)
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

    Channel::EventLoopRawPtr Channel::eventLoop() const
    {
        return eventLoop_;
    }

    EventType Channel::events() const
    {
        return events_;
    }

    bool Channel::isNoneEvent() const
    {
        return events_ == ENoneEvent;
    }

    void Channel::setEvents(EventType events)
    {
        events_ = events;
    }

    void Channel::addEvent(EventType event)
    {
        events_ |= event;
    }

    bool Channel::isRegistered() const
    {
        return isRegistered_;
    }

    void Channel::setRegistered(bool registered)
    {
        isRegistered_ = registered;
    }

    void Channel::setReadyEvents(EventType events)
    {
        readyEvents_ = events;
    }

    void Channel::addReadyEvent(EventType event)
    {
        readyEvents_ |= event;
    }

    void Channel::setChannelOption(const ChannelOption& channelOption)
    {
        SockOpt sockOpt = channelOption.sockOpt();
        try
        {
            switch (sockOpt)
            {
                case NBACKLOG:
                {
                    setBacklog(::std::any_cast<int32_t>(channelOption.get()));
                    break;
                }
                case NREUSEADDR:
                {
                    socket::setSockReuseAddr(fd(), ::std::any_cast<bool>(channelOption.get()));
                    break;
                }
                case NREUSEPORT:
                {
                    socket::setSockReusePort(fd(), ::std::any_cast<bool>(channelOption.get()));
                    break;
                }
                case NKEEPALIVE:
                {
                    socket::setSockKeepAlive(fd(), ::std::any_cast<bool>(channelOption.get()));
                    break;
                }
                case NTCPNODELAY:
                {
                    socket::setTcpNoDelay(fd(), ::std::any_cast<bool>(channelOption.get()));
                    break;
                }
                case NLINGER:
                {
                    auto linger = ::std::any_cast<int32_t>(channelOption.get());
                    socket::setSockLinger(fd(), {1, linger});
                    break;
                }
                case NTCPSNDBUF:
                case NUDPSNDBUF:
                {
                    socket::setSockSendBuf(fd(), ::std::any_cast<int32_t>(channelOption.get()));
                    break;
                }
                case NTCPRCVBUF:
                case NUDPRCVBUF:
                {
                    socket::setSockRecvBuf(fd(), ::std::any_cast<int32_t>(channelOption.get()));
                    break;
                }
                case InvalidSockOpt:
                default:
                    LOGS_FATAL << "Channel set invalid ChannelOption";
                    break;
            }
        }
        catch (const ::std::bad_any_cast& e)
        {
            LOGS_FATAL << "Channel set invalid type ChannelOption,";
        }
    }

    void Channel::setBacklog(int32_t backlog)
    {
        LOGS_FATAL << "Channel does not support setting backlog";
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