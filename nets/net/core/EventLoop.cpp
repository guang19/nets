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

// @brief The EventLoop manages all Channels and Pollers of the current thread,
// and constantly monitors Channel events and executes them.
// In addition, EventLoop is also responsible for executing user tasks and timing
// tasks

#include "nets/net/core/EventLoop.h"

#include <cassert>

#include "nets/base/exception/AbstractException.h"
#include "nets/base/log/Logger.h"
#include "nets/net/poller/Poller.h"

namespace nets
{
    namespace
    {
        constexpr TimeType kDefaultPollTimeout = 3000;
        constexpr TimeType kMinimumPollTimeout = 1;
        __thread EventLoop::EventLoopRawPtr tCurrentThreadEventLoop = nullptr;
    } // namespace

    EventLoop::EventLoop(EventLoopGroupRawPtr parent)
        : running_(false), threadId_(currentTid()), notifier_(::std::make_shared<NotifyChannel>(this)), activeChannels_(),
          poller_(PollerFactory::getPoller(this)), timerManager_(), pendingTasks_(), runningPendingTasks_(false), mutex_(),
          cv_(), parent_(parent)
    {
        assert(tCurrentThreadEventLoop == nullptr);
        // one loop per thread
        if (tCurrentThreadEventLoop != nullptr)
        {
            THROW_FMT(::std::runtime_error, "EventLoop there must be only one loop per thread");
        }
        else
        {
            tCurrentThreadEventLoop = this;
        }
        notifier_->addEvent(kReadEvent);
        registerChannel(notifier_);
        NETS_SYSTEM_LOG_INFO << "EventLoop one event loop is created in thread " << threadId_;
    }

    EventLoop::~EventLoop()
    {
        deregisterChannel(notifier_);
        tCurrentThreadEventLoop = nullptr;
        NETS_SYSTEM_LOG_INFO << "EventLoop one event loop is destroyed in thread " << threadId_;
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
                remainingExpiredTime == -1 ? kDefaultPollTimeout : ::std::min(remainingExpiredTime, kDefaultPollTimeout);
            poller_->poll(::std::max(kMinimumPollTimeout, timeout), activeChannels_);
            for (auto& channel: activeChannels_)
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
        // threadId_ == currentTid()
        return (this == tCurrentThreadEventLoop);
    }

    EventLoop::EventLoopRawPtr EventLoop::currentEventLoop() const
    {
        assert(isInCurrentEventLoop());
        return tCurrentThreadEventLoop;
    }

    EventLoop::EventLoopGroupRawPtr EventLoop::parent() const
    {
        return parent_;
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
        for (const auto& t: tmpTasks)
        {
            t();
        }
        runningPendingTasks_ = false;
    }
} // namespace nets