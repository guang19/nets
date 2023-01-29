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

// @brief C++17 based thread pool

#ifndef NETS_THREAD_POOL_H
#define NETS_THREAD_POOL_H

#include <atomic>
#include <cassert>
#include <future>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

#include "nets/base/BoundedBlockingQueue.h"
#include "nets/base/log/Logger.h"
#include "nets/base/ThreadHelper.h"

namespace nets
{
    class ThreadPool : Noncopyable
    {
    private:
        struct ThreadWrapper;
        using ThreadWrapperRawPtr = ThreadWrapper*;
        using ThreadWrapperPtr = ::std::unique_ptr<ThreadWrapper>;
        using ThreadPoolType = ::std::vector<ThreadWrapperPtr>;

    private:
        using TaskType = ::std::function<void()>;
        using MutexType = ::std::mutex;
        using LockGuardType = ::std::lock_guard<MutexType>;
        using UniqueLockType = ::std::unique_lock<MutexType>;
        using ConditionVariableType = ::std::condition_variable;
        using BlockingQueueType = BoundedBlockingQueue<TaskType>;
        using BlockingQueuePtr = ::std::unique_ptr<BlockingQueueType>;
        using ThreadPoolRawPtr = ThreadPool*;

    private:
        struct ThreadWrapper : Noncopyable
        {
        public:
            explicit ThreadWrapper(const StringType& threadName, bool isCoreThread, TaskType task,
                                   ThreadPoolRawPtr threadPool);
            ~ThreadWrapper() = default;

            void start(ThreadPoolRawPtr threadPoolPtr);

            bool isCoreThread_;
            StringType threadName_;
            TaskType task_;
            ::std::thread thread_;
        };

    public:
        explicit ThreadPool(Int32Type corePoolSize, Int32Type maxPoolSize, Int32Type maxQueueSize,
                            const StringType& name = kDefaultThreadPoolName,
                            TimeType keepAliveTime = kDefaultIdleKeepAliveTime);
        ~ThreadPool();

        void shutdown();

        inline bool isRunning() const
        {
            return isRunning(ctl_);
        }

        inline Int32Type numOfActiveThreads() const
        {
            return numOfActiveThreads(ctl_);
        }

        inline const StringType& name() const
        {
            return name_;
        }

        template <typename Fn, typename... Args>
        bool execute(Fn&& fn, Args&&... args);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Please note that if you need to use the returned future to get the result of the task execution, please use
        // try
        // {
        //     future.get();
        // }
        // catch(::std::future_error& e)
        // {
        //		// The thread pool was closed before the task was executed or the task was discarded
        // }
        // catch(::std::exception& e)
        // {
        // 		// The task throws an exception when it executes
        // }
        // to catch possible exceptions. The exceptions are as follows:
        // 1. An exception may occur when the submitted task is executed, and the promise sets the abnormal result;
        // 2. After the task is submitted, the thread pool shutdown before the task is executed;
        // 3. The thread pool is full and The task queue cannot receive new tasks, then the task will be discarded;
        template <typename Fn, typename... Args,
                  typename HasRet = typename ::std::enable_if<
                      !::std::is_void<typename ::std::invoke_result<Fn&&, Args&&...>::type>::value>::type>
        ::std::future<typename ::std::invoke_result<Fn&&, Args&&...>::type> submit(Fn&& fn, Args&&... args);

        template <typename Fn, typename... Args,
                  typename HasRet = typename ::std::enable_if<
                      ::std::is_void<typename ::std::invoke_result<Fn&&, Args&&...>::type>::value>::type>
        ::std::future<void> submit(Fn&& fn, Args&&... args);
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    private:
        void runThread(ThreadWrapperRawPtr threadWrapper);
        void releaseThread(ThreadWrapperRawPtr threadWrapperRawPtr);
        bool addThreadTask(const TaskType& task, bool isCore);

        inline bool isRunning(Int32Type ctl) const
        {
            return (ctl & ~kCountMask) == kRunning;
        }

        inline bool isShutdown(Int32Type ctl) const
        {
            return (ctl & ~kCountMask) == kShutdown;
        }

        inline Int32Type numOfActiveThreads(Int32Type ctl) const
        {
            return ctl & kCountMask;
        }

    private:
        static constexpr Int32Type kInt32Bits = 32;
        static constexpr Int32Type kCountBits = kInt32Bits - 2;
        // maximum active thread size
        // 00,111111111111111111111111111111
        static constexpr Int32Type kCountMask = (1 << kCountBits) - 1;
        // 01,000000000000000000000000000000
        static constexpr Int32Type kRunning = 1 << kCountBits;
        // 00,000000000000000000000000000000
        static constexpr Int32Type kShutdown = 0 << kCountBits;

        static constexpr TimeType kDefaultIdleKeepAliveTime = 30000;
        static constexpr char kDefaultThreadPoolName[] = "NetsThreadPool";

    private:
        // the numbers of core threads, once created, will be destroyed as the life cycle of the thread pool ends
        Int32Type corePoolSize_;
        // the maximum numbers of threads that the thread pool can hold
        Int32Type maximumPoolSize_;
        // time that idle threads can survive, unit: ms
        TimeType idleKeepAliveTime_;
        // task queue
        BlockingQueuePtr taskQueue_;
        ThreadPoolType threadPool_;
        StringType name_;
        // high 2bits represent thread pool status: 00 - shutdown; 01-running.
        // low 30bits represent thread pool active thread size.
        ::std::atomic<Int32Type> ctl_;
        MutexType mutex_;
        ConditionVariableType cv_;
    };

    template <typename Fn, typename... Args>
    bool ThreadPool::execute(Fn&& fn, Args&&... args)
    {
        TaskType task = ::std::bind(::std::forward<Fn>(fn), ::std::forward<Args>(args)...);
        Int32Type ctl = ctl_.load();
        assert(isRunning(ctl));
        if (isShutdown(ctl))
        {
            NETS_SYSTEM_LOG_WARN << "ThreadPool thread pool [" << name_ << "] has been shutdown";
            return false;
        }
        // if num of active threads less than num of corePoolSize
        if (numOfActiveThreads(ctl) < corePoolSize_)
        {
            if (addThreadTask(task, true))
            {
                return true;
            }
            // add task to core thread failed, update ctl
            ctl = ctl_.load();
        }
        // if task queue is not full, try push task to task queue
        if (isRunning(ctl) && taskQueue_->tryPush(task))
        {
            return true;
        }
        // task queue cannot receive new task, create non-core thread to execute task
        if (addThreadTask(task, false))
        {
            return true;
        }
        return false;
    }

    template <typename Fn, typename... Args, typename HasRet>
    ::std::future<typename ::std::invoke_result<Fn&&, Args&&...>::type> ThreadPool::submit(Fn&& fn, Args&&... args)
    {
        using RetType = typename ::std::invoke_result<Fn&&, Args&&...>::type;
        auto promise = ::std::make_shared<::std::promise<RetType>>();
        auto future = promise->get_future();
        ::std::function<RetType()> task = ::std::bind(::std::forward<Fn>(fn), ::std::forward<Args>(args)...);
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
                NETS_SYSTEM_LOG_ERROR << "ThreadPool exception caught during thread [" << currentThreadName()
                                      << "] execution in thread pool [" << name_ << "],reason is " << exception.what();
            }
            catch (...)
            {
                promise->set_exception(::std::current_exception());
                NETS_SYSTEM_LOG_ERROR << "ThreadPool unknown exception caught during thread [" << currentThreadName()
                                      << "] execution in thread pool [" << name_ << ']';
            }
        };
        assert(2 == promise.use_count());
        if (!execute(::std::move(promiseTask)))
        {
            promise->set_exception(::std::make_exception_ptr(::std::future_error(::std::future_errc::no_state)));
        }
        return future;
    }

    template <typename Fn, typename... Args, typename HasRet>
    ::std::future<void> ThreadPool::submit(Fn&& fn, Args&&... args)
    {
        auto promise = ::std::make_shared<::std::promise<void>>();
        auto future = promise->get_future();
        TaskType task = ::std::bind(::std::forward<Fn>(fn), ::std::forward<Args>(args)...);
        TaskType promiseTask = [this, promise, f = ::std::move(task)]() mutable
        {
            try
            {
                f();
                promise->set_value();
            }
            catch (const ::std::exception& exception)
            {
                promise->set_exception(::std::make_exception_ptr(exception));
                NETS_SYSTEM_LOG_ERROR << "ThreadPool exception caught during thread [" << currentThreadName()
                                      << "] execution in thread pool [" << name_ << "],reason is " << exception.what();
            }
            catch (...)
            {
                promise->set_exception(::std::current_exception());
                NETS_SYSTEM_LOG_ERROR << "ThreadPool unknown exception caught during thread [" << currentThreadName()
                                      << "] execution in thread pool [" << name_ << ']';
            }
        };
        assert(2 == promise.use_count());
        if (!execute(::std::move(promiseTask)))
        {
            promise->set_exception(::std::make_exception_ptr(::std::future_error(::std::future_errc::no_state)));
        }
        return future;
    }
} // namespace nets

#endif // NETS_THREAD_POOL_H