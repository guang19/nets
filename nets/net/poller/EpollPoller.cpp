//
// Created by guang19
//

#include "nets/net/poller/EpollPoller.h"

#include <cassert>
#include <stdexcept>
#include <unistd.h>

#include "nets/base/log/Logger.h"

namespace nets
{
    namespace
    {
        constexpr ::size_t kEpollEventInitialSize = 12;
        constexpr EpollPoller::EventList::size_type kMaxEpollEventSize = INT32_MAX;
    } // namespace

    EpollPoller::EpollPoller(EventLoopRawPtr eventLoop)
        : Poller(eventLoop), epollFd_(::epoll_create1(EPOLL_CLOEXEC)), events_(kEpollEventInitialSize)
    {
        assert(epollFd_ >= 0);
        if (epollFd_ < 0)
        {
            THROW_FMT(::std::runtime_error, "EpollPoller epoll create1 failed with result epollFd < 0");
        }
    }

    EpollPoller::~EpollPoller()
    {
        socket::closeFd(epollFd_);
    }

    void EpollPoller::poll(TimeType timeoutMs, ChannelList& activeChannels)
    {
        SizeType size = events_.size();
        ::int32_t numOfReadyEvents = ::epoll_wait(epollFd_, &events_[0], static_cast<::int32_t>(size), timeoutMs);
        if (numOfReadyEvents > 0)
        {
            NETS_SYSTEM_LOG_DEBUG << "EpollPoller epoll wait:" << numOfReadyEvents << " events";
            prepareChannelReadyEvents(numOfReadyEvents, activeChannels);
            if (static_cast<SizeType>(numOfReadyEvents) >= size)
            {
                size = size + (size >> 1);
                if (size >= kMaxEpollEventSize)
                {
                    size = kMaxEpollEventSize - 1;
                }
                events_.resize(size);
            }
        }
        else if (numOfReadyEvents < 0)
        {
            NETS_SYSTEM_LOG_ERROR << "EpollPoller epoll error,errNum=" << errno;
        }
    }

    void EpollPoller::prepareChannelReadyEvents(::int32_t numOfReadyEvents, ChannelList& activeChannels)
    {
        for (::int32_t i = 0; i < numOfReadyEvents; ++i)
        {
            auto channel = static_cast<ChannelRawPtr>(events_[i].data.ptr);
            activeChannels.push_back(channel);
            EventType revents = events_[i].events;
            channel->setReadyEvents(kNoneEvent);
            NETS_SYSTEM_LOG_DEBUG << "revents=" << revents << " [revents & EPOLLIN=" << (revents & EPOLLIN)
                       << "] [revents & EPOLLERR=" << (revents & EPOLLERR)
                       << "] [revents & EPOLLHUP=" << (revents & EPOLLHUP)
                       << "] [revents & EPOLLRDHUP=" << (revents & EPOLLRDHUP)
                       << "] [revents & EPOLLPRI=" << (revents & EPOLLPRI)
                       << "] [revents & EPOLLOUT=" << (revents & EPOLLOUT) << ']';
            // local may shutdown connection or read/write error
            if (revents & EPOLLIN && revents & (EPOLLERR | EPOLLHUP))
            {
                channel->addReadyEvent(kErrorEvent);
            }
            if (revents & (EPOLLIN | EPOLLPRI | EPOLLRDHUP))
            {
                channel->addReadyEvent(kReadEvent);
            }
            if (revents & EPOLLOUT)
            {
                channel->addReadyEvent(kWriteEvent);
            }
        }
    }

    bool EpollPoller::registerChannel(ChannelRawPtr channel)
    {
        if (!channel->isRegistered())
        {
            if (epollCtl(EPOLL_CTL_ADD, channel))
            {
                channel->setRegistered(true);
                return true;
            }
            return false;
        }
        else
        {
            return modifyChannel(channel);
        }
    }

    bool EpollPoller::modifyChannel(ChannelRawPtr channel)
    {
        if (channel->isRegistered())
        {
            if (!channel->isNoneEvent())
            {
                return epollCtl(EPOLL_CTL_MOD, channel);
            }
            else
            {
                return deregisterChannel(channel);
            }
        }
        else
        {
            return registerChannel(channel);
        }
    }

    bool EpollPoller::deregisterChannel(ChannelRawPtr channel)
    {
        channel->setRegistered(false);
        return epollCtl(EPOLL_CTL_DEL, channel);
    }

    bool EpollPoller::epollCtl(::int32_t opt, const ChannelRawPtr channel)
    {
        EpollEvent event {};
        FdType fd = channel->fd();
        event.data.fd = fd;
        event.data.ptr = channel;
        event.events = 0;
        EventType events = channel->events();
        if (events & kReadEvent)
        {
            event.events |= EPOLLIN;
        }
        if (events & kWriteEvent)
        {
            event.events |= EPOLLOUT;
        }
        if (0 == ::epoll_ctl(epollFd_, opt, fd, &event))
        {
            return true;
        }
        NETS_SYSTEM_LOG_ERROR << "EpollPoller epollCtl [" << epollOptToString(opt) << "] failed";
        return false;
    }

    const char* EpollPoller::epollOptToString(::int32_t opt)
    {
        switch (opt)
        {
            case EPOLL_CTL_ADD:
                return "ADD";
            case EPOLL_CTL_MOD:
                return "MOD";
            case EPOLL_CTL_DEL:
                return "DEL";
            default:
                return "unknown epoll ctl operation";
        }
    }
} // namespace nets