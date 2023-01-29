// MIT License
//
// Copyright (c) 2022 guang19
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

// @brief The encapsulation of Socket and Socket event processing

#include "nets/net/channel/Channel.h"

#include "nets/base/exception/AbstractException.h"
#include "nets/net/core/EventLoop.h"

namespace nets
{
    Channel::Channel(EventLoopRawPtr eventLoop)
        : events_(kNoneEvent), readyEvents_(kNoneEvent), isRegistered_(false), eventLoop_(eventLoop)
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
        return events_ == kNoneEvent;
    }

    bool Channel::hasReadEvent() const
    {
        return events_ & kReadEvent;
    }

    bool Channel::hasWriteEvent() const
    {
        return events_ & kWriteEvent;
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
                    setBacklog(::std::get<Int32Type>(value));
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
                    socket::setIpMulticastTTL(sockFd, ::std::get<Uint8Type>(value));
                    break;
                }
                case SockOption::IP6_MULTICAST_HOPS:
                {
                    socket::setIpv6MulticastHops(sockFd, ::std::get<Uint8Type>(value));
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

    void Channel::setBacklog(Int32Type backlog)
    {
        THROW_FMT(::std::runtime_error, "Channel does not support setting backlog");
    }

    void Channel::handleEvent()
    {
        if (readyEvents_ & kErrorEvent)
        {
            handleErrorEvent();
        }
        if (readyEvents_ & kReadEvent)
        {
            handleReadEvent();
        }
        if (readyEvents_ & kWriteEvent)
        {
            handleWriteEvent();
        }
    }

    void Channel::handleReadEvent()
    {
        NETS_SYSTEM_LOG_DEBUG << "Channel::handleReadEvent";
    }

    void Channel::handleWriteEvent()
    {
        NETS_SYSTEM_LOG_DEBUG << "Channel::handleWriteEvent";
    }

    void Channel::handleErrorEvent()
    {
        NETS_SYSTEM_LOG_DEBUG << "Channel::handleErrorEvent";
    }
} // namespace nets