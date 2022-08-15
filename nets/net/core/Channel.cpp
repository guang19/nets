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

    void Channel::setEvents(EventType events)
    {
        events_ = events;
    }

    void Channel::addEvent(EventType event)
    {
        events_ |= event;
    }

    bool Channel::isNoneEvent() const
    {
        return events_ == ENoneEvent;
    }

    bool Channel::hasReadEvent() const
    {
        return events_ & EReadEvent;
    }

    bool Channel::hasWriteEvent() const
    {
        return events_ & EWriteEvent;
    }

    void Channel::setReadyEvents(EventType events)
    {
        readyEvents_ = events;
    }

    void Channel::addReadyEvent(EventType event)
    {
        readyEvents_ |= event;
    }

    bool Channel::isRegistered() const
    {
        return isRegistered_;
    }

    void Channel::setRegistered(bool registered)
    {
        isRegistered_ = registered;
    }

    void Channel::setChannelOption(const ChannelOption& channelOption)
    {
        SockOpt sockOpt = channelOption.sockOpt();
        try
        {
            switch (sockOpt)
            {
                case SockOpt::NBACKLOG:
                {
                    setBacklog(::std::any_cast<int32_t>(channelOption.get()));
                    break;
                }
                case SockOpt::NREUSE_ADDR:
                {
                    socket::setSockReuseAddr(fd(), ::std::any_cast<bool>(channelOption.get()));
                    break;
                }
                case SockOpt::NREUSE_PORT:
                {
                    socket::setSockReusePort(fd(), ::std::any_cast<bool>(channelOption.get()));
                    break;
                }
                case SockOpt::NKEEP_ALIVE:
                {
                    socket::setSockKeepAlive(fd(), ::std::any_cast<bool>(channelOption.get()));
                    break;
                }
                case SockOpt::NTCP_NODELAY:
                {
                    socket::setTcpNoDelay(fd(), ::std::any_cast<bool>(channelOption.get()));
                    break;
                }
                case SockOpt::NLINGER:
                {
                    auto linger = ::std::any_cast<int32_t>(channelOption.get());
                    socket::setSockLinger(fd(), {1, linger});
                    break;
                }
                case SockOpt::NTCP_SNDBUF:
                case SockOpt::NUDP_SNDBUF:
                {
                    socket::setSockSendBuf(fd(), ::std::any_cast<int32_t>(channelOption.get()));
                    break;
                }
                case SockOpt::NTCP_RCVBUF:
                case SockOpt::NUDP_RCVBUF:
                {
                    socket::setSockRecvBuf(fd(), ::std::any_cast<int32_t>(channelOption.get()));
                    break;
                }
                case SockOpt::INVALID_SOCKOPT:
                default:
                    THROW_FMT(::std::invalid_argument, "Channel set invalid ChannelOption");
                    break;
            }
        }
        catch (const ::std::bad_any_cast& e)
        {
            THROW_FMT(::std::invalid_argument, "Channel set invalid ChannelOption,cause %s", e.what());
        }
    }

    void Channel::setBacklog(int32_t backlog)
    {
        THROW_FMT(::std::runtime_error, "Channel does not support setting backlog");
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