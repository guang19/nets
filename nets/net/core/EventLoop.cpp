//
// Created by guang19 on 2022/5/15.
//

#include "nets/net/core/EventLoop.h"

#include <cassert>

#include "nets/base/log/Logging.h"
#include "nets/net/poller/Poller.h"

namespace nets::net
{
    namespace
    {
        using TimeType = ::time_t;
        constexpr TimeType PollTimeout = 10000;
        __thread EventLoop::EventLoopRawPtr CurrentThreadEventLoop = nullptr;
    } // namespace

    EventLoop::EventLoop()
        : running_(false), threadId_(nets::base::currentTid()), notifier_(::std::make_shared<NotifyChannel>(this)),
          poller_(PollerFactory::getPoller(this))
    {
        assert(CurrentThreadEventLoop == nullptr);
        // one loop per thread
        if (CurrentThreadEventLoop != nullptr)
        {
            LOGS_FATAL << "EventLoop there must be only one loop per thread";
        }
        else
        {
            CurrentThreadEventLoop = this;
        }
        notifier_->addEvent(EReadEvent);
        registerChannel(notifier_);
        LOGS_INFO << "EventLoop one event loop is created in thread " << threadId_;
    }

    EventLoop::~EventLoop() {}

    void EventLoop::run()
    {
        assert(!running_);
        running_ = true;
        while (running_)
        {
            activeChannels_.clear();
            poller_->poll(PollTimeout, activeChannels_);
            for (auto& channel: activeChannels_)
            {
                channel->handleEvent();
            }
            runPendingTasks();
        }
        assert(!running_);
    }

    void EventLoop::runPendingTasks()
    {
        TaskList tmpTasks {};
        {
            LockGuardType lock(mutex_);
            tmpTasks.swap(pendingTasks_);
        }
        for (const auto& t: tmpTasks)
        {
            t();
        }
    }

    void EventLoop::shutdown() {}

    bool EventLoop::isInCurrentEventLoop() const
    {
        // threadId_ == nets::base::currentTid()
        return (this == CurrentThreadEventLoop);
    }

    EventLoop::EventLoopRawPtr EventLoop::currentEventLoop() const
    {
        assert(isInCurrentEventLoop());
        return CurrentThreadEventLoop;
    }

    bool EventLoop::registerChannel(const ChannelPtr& channel)
    {
        assert(channels_.find(channel->fd()) == channels_.end());
        if (poller_->registerChannel(channel.get()))
        {
            // use_count + 1
            channels_[channel->fd()] = channel;
            return true;
        }
        assert(channels_.find(channel->fd()) != channels_.end());
        return false;
    }

    bool EventLoop::modifyChannel(const ChannelPtr& channel)
    {
        assert(channels_.find(channel->fd()) != channels_.end());
        return poller_->modifyChannel(channel.get());
    }

    void EventLoop::deregisterChannel(const ChannelPtr& channel)
    {
        assert(channels_.find(channel->fd()) != channels_.end());
        channels_.erase(channel->fd());
        poller_->deregisterChannel(channel.get());
        assert(channels_.find(channel->fd()) == channels_.end());
    }
} // namespace nets::net