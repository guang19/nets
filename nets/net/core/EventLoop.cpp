//
// Created by guang19
//

#include "nets/net/core/EventLoop.h"

#include <cassert>

#include "nets/base/log/Logging.h"
#include "nets/net/poller/Poller.h"

namespace nets::net
{
    namespace
    {
        constexpr EventLoop::TimeType gDefaultPollTimeout = 3000;
        constexpr EventLoop::TimeType gMinimumPollTimeout = 1;
        __thread EventLoop::EventLoopRawPtr gCurrentThreadEventLoop = nullptr;
    } // namespace

    EventLoop::EventLoop()
        : running_(false), threadId_(nets::base::currentTid()), notifier_(::std::make_shared<NotifyChannel>(this)),
          activeChannels_(), poller_(PollerFactory::getPoller(this)), timerManager_(), pendingTasks_(),
          runningPendingTasks_(false), mutex_()
    {
        assert(gCurrentThreadEventLoop == nullptr);
        // one loop per thread
        if (gCurrentThreadEventLoop != nullptr)
        {
            THROW_FMT(::std::runtime_error, "EventLoop there must be only one loop per thread");
        }
        else
        {
            gCurrentThreadEventLoop = this;
        }
        notifier_->addEvent(gReadEvent);
        registerChannel(notifier_);
        LOGS_INFO << "EventLoop one event loop is created in thread " << threadId_;
    }

    EventLoop::~EventLoop()
    {
        deregisterChannel(notifier_);
        gCurrentThreadEventLoop = nullptr;
        LOGS_INFO << "EventLoop one event loop is destroyed in thread " << threadId_;
    }

    void EventLoop::run()
    {
        assert(!running_);
        running_ = true;
        while (running_)
        {
            timerManager_.update();
            runPendingTasks();
            activeChannels_.clear();
            TimeType remainingExpiredTime = timerManager_.nearestTimerRemainingExpiredTime();
            TimeType timeout =
                remainingExpiredTime == -1 ? gDefaultPollTimeout : ::std::min(remainingExpiredTime, gDefaultPollTimeout);
            poller_->poll(::std::max(gMinimumPollTimeout, timeout), activeChannels_);
            for (auto& channel : activeChannels_)
            {
                channel->handleEvent();
            }
        }
        assert(!running_);
    }

    void EventLoop::shutdown()
    {
        running_ = false;
        if (!isInCurrentEventLoop())
        {
            notifier_->notify();
        }
    }

    bool EventLoop::isRunning() const
    {
        return running_;
    }

    bool EventLoop::isInCurrentEventLoop() const
    {
        // threadId_ == nets::base::currentTid()
        return (this == gCurrentThreadEventLoop);
    }

    EventLoop::EventLoopRawPtr EventLoop::currentEventLoop() const
    {
        assert(isInCurrentEventLoop());
        return gCurrentThreadEventLoop;
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
        poller_->deregisterChannel(channel.get());
        channels_.erase(channel->fd());
        assert(channels_.find(channel->fd()) == channels_.end());
    }

    void EventLoop::cancelScheduleTask(const TimerId& timerId)
    {
        timerManager_.removeTimer(timerId);
    }

    void EventLoop::runPendingTasks()
    {
        TaskList tmpTasks {};
        runningPendingTasks_ = true;
        {
            LockGuardType lock(mutex_);
            tmpTasks.swap(pendingTasks_);
        }
        for (const auto& t : tmpTasks)
        {
            t();
        }
        runningPendingTasks_ = false;
    }
} // namespace nets::net