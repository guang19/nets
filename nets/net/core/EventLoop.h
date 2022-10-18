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

#ifndef NETS_EVENT_LOOP_H
#define NETS_EVENT_LOOP_H

#include <atomic>
#include <cassert>
#include <functional>
#include <future>
#include <memory>
#include <vector>

#include "nets/base/log/Logger.h"
#include "nets/base/Noncopyable.h"
#include "nets/base/ThreadHelper.h"
#include "nets/base/timer/TimerManager.h"
#include "nets/net/channel/NotifyChannel.h"

namespace nets
{
    class Channel;
    class Poller;
    class PollerFactory;
    class EventLoopGroup;

    class EventLoop : Noncopyable
    {
    public:
        using TaskType = ::std::function<void()>;
        using TaskList = ::std::vector<TaskType>;
        using MutexType = ::std::mutex;
        using LockGuardType = ::std::lock_guard<MutexType>;
        using ConditionVariableType = ::std::condition_variable;
        using ChannelRawPtr = Channel*;
        using ChannelPtr = ::std::shared_ptr<Channel>;
        // cannot use shared_ptr
        using ChannelList = ::std::vector<ChannelRawPtr>;
        using ChannelMap = ::std::unordered_map<FdType, ChannelPtr>;
        using NotifyChannelPtr = ::std::shared_ptr<NotifyChannel>;
        using PollerPtr = ::std::unique_ptr<Poller>;
        using EventLoopRawPtr = EventLoop*;
        using EventLoopGroupRawPtr = EventLoopGroup*;

    public:
        EventLoop(EventLoopGroupRawPtr parent);
        ~EventLoop();

    public:
        void run();
        void shutdown();
        bool isRunning() const;
        bool isInCurrentEventLoop() const;
        EventLoopRawPtr currentEventLoop() const;
        EventLoopGroupRawPtr parent() const;

        bool registerChannel(const ChannelPtr& channel);
        bool modifyChannel(const ChannelPtr& channel);
        void deregisterChannel(const ChannelPtr& channel);

        template <typename Fn, typename... Args>
        void addTask(Fn&& func, Args&&... args);

        template <typename Fn, typename... Args>
        void execute(Fn&& func, Args&&... args);

        template <typename Fn, typename... Args,
                  typename HasRet = typename ::std::enable_if<
                      !::std::is_void<typename ::std::invoke_result<Fn&&, Args&&...>::type>::value>::type>
        ::std::future<typename ::std::invoke_result<Fn&&, Args&&...>::type> submit(Fn&& func, Args&&... args);

        template <typename Fn, typename... Args,
                  typename HasRet = typename ::std::enable_if<
                      ::std::is_void<typename ::std::invoke_result<Fn&&, Args&&...>::type>::value>::type>
        ::std::future<void> submit(Fn&& func, Args&&... args);

        // time unit: ms
        template <typename Fn, typename... Args>
        TimerId schedule(TimeType delay, Fn&& fn, Args&&... args);

        // time unit: ms
        template <typename Fn, typename... Args>
        TimerId scheduleAtFixedRate(TimeType initDelay, TimeType interval, Fn&& fn, Args&&... args);

        // time unit: ms
        template <typename Fn, typename... Args>
        TimerId scheduleAtFixedDelay(TimeType initDelay, TimeType delay, Fn&& fn, Args&&... args);

        void cancelScheduleTask(const TimerId& timerId);

    private:
        void runPendingTasks();

    private:
        ::std::atomic_bool running_;
        const ::pid_t threadId_;
        NotifyChannelPtr notifier_;
        ChannelMap channels_;
        ChannelList activeChannels_;
        PollerPtr poller_;
        TimerManager timerManager_;
        TaskList pendingTasks_;
        bool runningPendingTasks_;
        MutexType mutex_;
        ConditionVariableType cv_;
        EventLoopGroupRawPtr parent_;
    };

    template <typename Fn, typename... Args>
    void EventLoop::addTask(Fn&& func, Args&&... args)
    {
        TaskType task = ::std::bind(::std::forward<Fn>(func), ::std::forward<Args>(args)...);
        {
            LockGuardType lock(mutex_);
            pendingTasks_.push_back(::std::move(task));
        }
        if (!isInCurrentEventLoop() || runningPendingTasks_)
        {
            notifier_->notify();
        }
    }

    template <typename Fn, typename... Args>
    void EventLoop::execute(Fn&& func, Args&&... args)
    {
        if (isInCurrentEventLoop())
        {
            ::std::forward<Fn>(func)(::std::forward<Args>(args)...);
        }
        else
        {
            addTask(::std::forward<Fn>(func), ::std::forward<Args>(args)...);
        }
    }

    template <typename Fn, typename... Args, typename HasRet>
    ::std::future<typename ::std::invoke_result<Fn&&, Args&&...>::type> EventLoop::submit(Fn&& func, Args&&... args)
    {
        using RetType = typename ::std::invoke_result<Fn&&, Args&&...>::type;
        auto promise = ::std::make_shared<::std::promise<RetType>>();
        auto future = promise->get_future();
        ::std::function<RetType()> task = ::std::bind(::std::forward<Fn>(func), ::std::forward<Args>(args)...);
        TaskType promiseTask = [this, promise, f = ::std::move(task)]() mutable
        {
            assert(promise.use_count() > 0);
            try
            {
                RetType result = f();
                promise->set_value(result);
            }
            catch (const ::std::exception& exception)
            {
                promise->set_exception(::std::make_exception_ptr(exception));
                NETS_SYSTEM_LOG_ERROR << "EventLoop exception caught during thread [" << currentTid()
                           << "] execution in event loop thread [" << threadId_ << "],reason is " << exception.what();
            }
            catch (...)
            {
                promise->set_exception(::std::current_exception());
                NETS_SYSTEM_LOG_ERROR << "EventLoop exception caught during thread [" << currentTid()
                           << "] execution in event loop thread [" << threadId_ << "]";
            }
        };
        assert(2 == promise.use_count());
        execute(::std::move(promiseTask));
        return future;
    }

    template <typename Fn, typename... Args, typename HasRet>
    ::std::future<void> EventLoop::submit(Fn&& func, Args&&... args)
    {
        auto promise = ::std::make_shared<::std::promise<void>>();
        auto future = promise->get_future();
        ::std::function<void()> task = ::std::bind(::std::forward<Fn>(func), ::std::forward<Args>(args)...);
        TaskType promiseTask = [this, promise, f = ::std::move(task)]() mutable
        {
            assert(promise.use_count() > 0);
            try
            {
                f();
                promise->set_value();
            }
            catch (const ::std::exception& exception)
            {
                promise->set_exception(::std::make_exception_ptr(exception));
                NETS_SYSTEM_LOG_ERROR << "EventLoop exception caught during thread [" << currentTid()
                           << "] execution in event loop thread [" << threadId_ << "],reason is " << exception.what();
            }
            catch (...)
            {
                promise->set_exception(::std::current_exception());
                NETS_SYSTEM_LOG_ERROR << "EventLoop exception caught during thread [" << currentTid()
                           << "] execution in event loop thread [" << threadId_ << "]";
            }
        };
        assert(2 == promise.use_count());
        execute(::std::move(promiseTask));
        return future;
    }

    template <typename Fn, typename... Args>
    TimerId EventLoop::schedule(TimeType delay, Fn&& fn, Args&&... args)
    {
        return timerManager_.addTimer(Timestamp::now().plusMilliseconds(delay), 1, 0, false, ::std::forward<Fn>(fn),
                                      ::std::forward<Args>(args)...);
    }

    template <typename Fn, typename... Args>
    TimerId EventLoop::scheduleAtFixedRate(TimeType initDelay, TimeType interval, Fn&& fn, Args&&... args)
    {
        return timerManager_.addTimer(Timestamp::now().plusMilliseconds(initDelay), Timer::kRepeatForever, interval, false,
                                      ::std::forward<Fn>(fn), ::std::forward<Args>(args)...);
    }

    template <typename Fn, typename... Args>
    TimerId EventLoop::scheduleAtFixedDelay(TimeType initDelay, TimeType delay, Fn&& fn, Args&&... args)
    {
        return timerManager_.addTimer(Timestamp::now().plusMilliseconds(initDelay), Timer::kRepeatForever, delay, true,
                                      ::std::forward<Fn>(fn), ::std::forward<Args>(args)...);
    }
} // namespace nets

#endif // NETS_EVENT_LOOP_H