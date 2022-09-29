//
// Created by guang19
//

#include "nets/net/channel/Channel.h"

#include "nets/net/core/EventLoop.h"

namespace nets::net
{
    Channel::Channel(EventLoopRawPtr eventLoop)
        : events_(gNoneEvent), readyEvents_(gNoneEvent), isRegistered_(false), eventLoop_(eventLoop)
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
        return events_ == gNoneEvent;
    }

    bool Channel::hasReadEvent() const
    {
        return events_ & gReadEvent;
    }

    bool Channel::hasWriteEvent() const
    {
        return events_ & gWriteEvent;
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

    void Channel::setChannelOption(SockOption sockOption, const ChannelOption::ValueType& value)
    {
        FdType sockFd = fd();
        try
        {
            switch (sockOption)
            {
                case SockOption::BACKLOG:
                {
                    setBacklog(::std::get<::int32_t>(value));
                    break;
                }
                case SockOption::REUSE_ADDR:
                {
                    socket::setSockReuseAddr(sockFd, ::std::get<bool>(value));
                    break;
                }
                case SockOption::REUSE_PORT:
                {
                    socket::setSockReusePort(sockFd, ::std::get<bool>(value));
                    break;
                }
                case SockOption::KEEP_ALIVE:
                {
                    socket::setSockKeepAlive(sockFd, ::std::get<bool>(value));
                    break;
                }
                case SockOption::KEEP_IDLE:
                {
                    socket::setSockKeepIdle(sockFd, ::std::get<OptValType>(value));
                    break;
                }
                case SockOption::KEEP_CNT:
                {
                    socket::setSockKeepCnt(sockFd, ::std::get<OptValType>(value));
                    break;
                }
                case SockOption::KEEP_INTVL:
                {
                    socket::setSockKeepIntvl(sockFd, ::std::get<OptValType>(value));
                    break;
                }
                case SockOption::TCP_NODELAY:
                {
                    socket::setTcpNoDelay(sockFd, ::std::get<bool>(value));
                    break;
                }
                case SockOption::LINGER:
                {
                    socket::setSockLinger(sockFd, ::std::get<SockLinger>(value));
                    break;
                }
                case SockOption::BROADCAST:
                {
                    socket::setSockBroadcast(sockFd, ::std::get<bool>(value));
                    break;
                }
                case SockOption::IP4_MULTICAST_IF:
                {
                    socket::setIpMulticastIf(sockFd, ::std::get<StringType>(value));
                    break;
                }
                case SockOption::IP6_MULTICAST_IF:
                {
                    socket::setIpv6MulticastIf(sockFd, ::std::get<StringType>(value));
                    break;
                }
                case SockOption::IP4_MULTICAST_TTL:
                {
                    socket::setIpMulticastTTL(sockFd, ::std::get<uint8_t>(value));
                    break;
                }
                case SockOption::IP6_MULTICAST_HOPS:
                {
                    socket::setIpv6MulticastHops(sockFd, ::std::get<uint8_t>(value));
                    break;
                }
                case SockOption::IP4_MULTICAST_LOOP:
                {
                    socket::setIpMulticastLoop(sockFd, ::std::get<bool>(value));
                    break;
                }
                case SockOption::IP6_MULTICAST_LOOP:
                {
                    socket::setIpv6MulticastLoop(sockFd, ::std::get<bool>(value));
                    break;
                }
                case SockOption::SNDBUF:
                {
                    socket::setSockSendBuf(sockFd, ::std::get<OptValType>(value));
                    break;
                }
                case SockOption::RCVBUF:
                {
                    socket::setSockRecvBuf(sockFd, ::std::get<OptValType>(value));
                    break;
                }
                default:
                    THROW_FMT(::std::invalid_argument, "Channel set invalid ChannelOption");
                    break;
            }
        }
        catch (const ::std::bad_variant_access& e)
        {
            THROW_FMT(::std::invalid_argument, "Channel set invalid ChannelOption,cause %s", e.what());
        }
    }

    void Channel::setBacklog(::int32_t backlog)
    {
        THROW_FMT(::std::runtime_error, "Channel does not support setting backlog");
    }

    void Channel::handleEvent()
    {
        if (readyEvents_ & gErrorEvent)
        {
            handleErrorEvent();
        }
        if (readyEvents_ & gReadEvent)
        {
            handleReadEvent();
        }
        if (readyEvents_ & gWriteEvent)
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