//
// Created by guang19
//

#ifndef NETS_NET_EVENT_LOOP_H
#define NETS_NET_EVENT_LOOP_H

#include <atomic>
#include <cassert>
#include <functional>
#include <future>
#include <memory>
#include <vector>

#include "nets/base/log/Logging.h"
#include "nets/base/Noncopyable.h"
#include "nets/base/ThreadHelper.h"
#include "nets/base/timer/TimerManager.h"
#include "nets/net/core/NotifyChannel.h"

namespace nets::net
{
    class Channel;
    class Poller;
    class PollerFactory;

    class EventLoop : nets::base::Noncopyable
    {
    public:
        using TaskType = ::std::function<void()>;
        using TaskList = ::std::vector<TaskType>;
        using MutexType = ::std::mutex;
        using LockGuardType = ::std::lock_guard<MutexType>;
        using ChannelRawPtr = Channel*;
        using ChannelPtr = ::std::shared_ptr<Channel>;
        // cannot use shared_ptr
        using ChannelList = ::std::vector<ChannelRawPtr>;
        using ChannelMap = ::std::unordered_map<FdType, ChannelPtr>;
        using NotifyChannelPtr = ::std::shared_ptr<NotifyChannel>;
        using PollerPtr = ::std::unique_ptr<Poller>;
        using TimeType = typename nets::base::TimerManager::TimeType;
        using Timer = typename nets::base::Timer;
        using TimerId = typename nets::base::Timer::TimerId;
        using Timestamp = typename nets::base::Timestamp;
        using TimerManager = typename nets::base::TimerManager;
        using EventLoopRawPtr = EventLoop*;

    public:
        EventLoop();
        ~EventLoop();

    public:
        void run();
        void shutdown();
        bool isInCurrentEventLoop() const;
        EventLoopRawPtr currentEventLoop() const;
        
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
        ::std::atomic_bool running_ {false};
        const ::pid_t threadId_ {0};
        NotifyChannelPtr notifier_ {nullptr};
        ChannelMap channels_ {};
        ChannelList activeChannels_ {nullptr};
        PollerPtr poller_ {nullptr};
        TimerManager timerManager_ {};
        TaskList pendingTasks_ {};
        bool runningPendingTasks_ {false};
        MutexType mutex_ {};
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
                LOGS_ERROR << "EventLoop exception caught during thread [" << nets::base::currentTid()
                           << "] execution in event loop thread [" << threadId_ << "],reason " << exception.what();
            }
            catch (...)
            {
                promise->set_exception(::std::current_exception());
                LOGS_ERROR << "EventLoop exception caught during thread [" << nets::base::currentTid()
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
                LOGS_ERROR << "EventLoop exception caught during thread [" << nets::base::currentTid()
                           << "] execution in event loop thread [" << threadId_ << "],reason " << exception.what();
            }
            catch (...)
            {
                promise->set_exception(::std::current_exception());
                LOGS_ERROR << "EventLoop exception caught during thread [" << nets::base::currentTid()
                           << "] execution in event loop thread [" << threadId_ << "]";
            }
        };
        assert(2 == promise.use_count());
        execute(::std::move(promiseTask));
        return future;
    }

    template <typename Fn, typename... Args>
    EventLoop::TimerId EventLoop::schedule(TimeType delay, Fn&& fn, Args&&... args)
    {
        return timerManager_.addTimer(Timestamp::now().plusMilliseconds(delay), 1, 0, false, ::std::forward<Fn>(fn),
                                      ::std::forward<Args>(args)...);
    }

    template <typename Fn, typename... Args>
    EventLoop::TimerId EventLoop::scheduleAtFixedRate(TimeType initDelay, TimeType interval, Fn&& fn, Args&&... args)
    {
        return timerManager_.addTimer(Timestamp::now().plusMilliseconds(initDelay), Timer::RepeatForever, interval, false,
                                      ::std::forward<Fn>(fn), ::std::forward<Args>(args)...);
    }

    template <typename Fn, typename... Args>
    EventLoop::TimerId EventLoop::scheduleAtFixedDelay(TimeType initDelay, TimeType delay, Fn&& fn, Args&&... args)
    {
        return timerManager_.addTimer(Timestamp::now().plusMilliseconds(initDelay), Timer::RepeatForever, delay, true,
                                      ::std::forward<Fn>(fn), ::std::forward<Args>(args)...);
    }
} // namespace nets::net

#endif // NETS_NET_EVENT_LOOP_H