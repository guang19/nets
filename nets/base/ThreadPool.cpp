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

// @brief Thread pool

#include "nets/base/ThreadPool.h"

#include "nets/base/exception/AbstractException.h"

namespace nets
{
    ThreadPool::ThreadWrapper::ThreadWrapper(const StringType& threadName, bool isCoreThread, TaskType task,
                                             ThreadPoolRawPtr threadPoolPtr)
        : isCoreThread_(isCoreThread), threadName_(threadName), task_(std::move(task)),
          thread_(&ThreadWrapper::start, this, threadPoolPtr)
    {
        if (thread_.joinable())
        {
            thread_.detach();
        }
    }

    void ThreadPool::ThreadWrapper::start(ThreadPoolRawPtr threadPool)
    {
        assert(threadPool != nullptr);
        setCurrentThreadName(threadName_.c_str());
        threadPool->runThread(this);
    }

    ThreadPool::ThreadPool(Int32Type corePoolSize, Int32Type maximumPoolSize, Int32Type maxQueueSize, const StringType& name,
                           TimeType idleKeepAliveTime)
        : corePoolSize_(corePoolSize), maximumPoolSize_(maximumPoolSize), idleKeepAliveTime_(idleKeepAliveTime),
          taskQueue_(::std::make_unique<BlockingQueueType>(maxQueueSize)), threadPool_(), name_(name), ctl_(kRunning),
          mutex_(), cv_()
    {
        if (corePoolSize_ <= 0 || corePoolSize_ > maximumPoolSize_)
        {
            THROW_FMT(::std::invalid_argument,
                      "ThreadPool corePoolSize must be greater than 0 and maximumPoolSize must be greater than "
                      "corePoolSize.corePoolSize=%d,maximumPoolSize=%d",
                      corePoolSize_, maximumPoolSize_);
        }
        threadPool_.reserve(maximumPoolSize_);
        NETS_SYSTEM_LOG_INFO << "ThreadPool thread pool [" << name_ << "] init success";
    }

    ThreadPool::~ThreadPool()
    {
        shutdown();
    }

    void ThreadPool::shutdown()
    {
        if (isShutdown(ctl_))
        {
            NETS_SYSTEM_LOG_DEBUG << "ThreadPool thread pool [" << name_ << "] has been shutdown";
            return;
        }
        // cas
        for (;;)
        {
            Int32Type ctl = ctl_.load();
            // set state to shutdown
            if (ctl_.compare_exchange_strong(ctl, (kShutdown | (ctl & kCountMask))))
            {
                break;
            }
        }
        // notify blocking thread
        taskQueue_->notifyBlockingThread();
        UniqueLockType lock(mutex_);
        cv_.wait(lock,
                 [this]() -> bool
                 {
                     return numOfActiveThreads(ctl_) == 0;
                 });
        assert(threadPool_.empty());
        // if thread pool has no thread takes task from task queue, it needs to be deleted manually
        TaskType tmpTask = nullptr;
        while (!taskQueue_->isEmpty())
        {
            taskQueue_->tryPop(tmpTask);
        }
        assert(taskQueue_->isEmpty());
           NETS_SYSTEM_LOG_INFO << "ThreadPool thread pool [" << name_ << "] shutdown success";
    }

    void ThreadPool::runThread(ThreadWrapperRawPtr threadWrapper)
    {
        if (threadWrapper->task_ != nullptr)
        {
            threadWrapper->task_();
            threadWrapper->task_ = nullptr;
        }
        ::std::function<bool()> shutdown = [this]() -> bool
        {
            return isShutdown(ctl_);
        };
        while (isRunning(ctl_))
        {
            // core thread blocked wait
            if (threadWrapper->isCoreThread_)
            {
                taskQueue_->take(threadWrapper->task_, shutdown);
            }
            else
            {
                // non-core thread wait timeout
                if (!taskQueue_->take(threadWrapper->task_, idleKeepAliveTime_, shutdown))
                {
                    break;
                }
            }
            if (threadWrapper->task_ != nullptr)
            {
                threadWrapper->task_();
                threadWrapper->task_ = nullptr;
            }
        }
        releaseThread(threadWrapper);
    }

    void ThreadPool::releaseThread(ThreadWrapperRawPtr threadWrapper)
    {
        LockGuardType lock(mutex_);
        for (auto it = threadPool_.begin(), end = threadPool_.end(); it != end; ++it)
        {
            if (it->get() == threadWrapper)
            {
                threadPool_.erase(it);
                --ctl_;
                cv_.notify_all();
                return;
            }
        }
    }

    bool ThreadPool::addThreadTask(const TaskType& task, bool isCore)
    {
        // cas
        for (;;)
        {
            Int32Type ctl = ctl_.load();
            if (isShutdown(ctl))
            {
                return false;
            }
            if (numOfActiveThreads(ctl) >= (isCore ? corePoolSize_ : maximumPoolSize_))
            {
                return false;
            }
            if (ctl_.compare_exchange_strong(ctl, ctl + 1))
            {
                LockGuardType lock(mutex_);
                if (isRunning(ctl_))
                {
                    // set thread name
                    char threadName[kThreadNameMaxLength] = {0};
                    ::snprintf(threadName, kThreadNameMaxLength, "%s-Thread-%u", name_.c_str(), numOfActiveThreads(ctl_));
                    threadPool_.emplace_back(new ThreadWrapper(threadName, isCore, task, this));
                    return true;
                }
                else
                {
                    --ctl_;
                    cv_.notify_all();
                    return false;
                }
            }
        }
    }
} // namespace nets