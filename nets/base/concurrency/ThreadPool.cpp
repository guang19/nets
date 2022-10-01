//
// Created by guang19
//

#include "nets/base/concurrency/ThreadPool.h"

namespace nets
{
    ThreadPool::ThreadWrapper::ThreadWrapper(const StringType& threadName, bool isCoreThread, TaskType task,
                                             ThreadPoolRawPtr threadPoolPtr)
        : threadName_(threadName), isCoreThread_(isCoreThread), task_(std::move(task)),
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

    ThreadPool::ThreadPool(IntType corePoolSize, IntType maximumPoolSize, IntType maxQueueSize, const StringType& name,
                           TimeType idleKeepAliveTime)
        : corePoolSize_(corePoolSize), maximumPoolSize_(maximumPoolSize), idleKeepAliveTime_(idleKeepAliveTime),
          taskQueue_(::std::make_unique<BlockingQueueType>(maxQueueSize)), threadPool_(), name_(name), ctl_(gRunning),
          mutex_(), cv_()
    {
        if (corePoolSize_ <= 0 || corePoolSize_ > maximumPoolSize_)
        {
            THROW_FMT(::std::invalid_argument,
                      "ThreadPool corePoolSize must be greater than 0 and maximumPoolSize must be greater than "
                      "corePoolSize.corePoolSize=%u,maximumPoolSize=%u",
                      corePoolSize_, maximumPoolSize_);
        }
        threadPool_.reserve(maximumPoolSize_);
        LOGS_INFO << "ThreadPool thread pool [" << name_ << "] init success";
    }

    ThreadPool::~ThreadPool()
    {
        shutdown();
    }

    void ThreadPool::shutdown()
    {
        if (isShutdown(ctl_))
        {
            LOGS_DEBUG << "ThreadPool thread pool [" << name_ << "] has been shutdown";
            return;
        }
        // cas
        while (true)
        {
            IntType ctl = ctl_.load();
            // set state to shutdown
            if (ctl_.compare_exchange_strong(ctl, (gShutdown | (ctl & gCountMask))))
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
        LOGS_INFO << "ThreadPool thread pool [" << name_ << "] shutdown success";
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
        while (true)
        {
            IntType ctl = ctl_.load();
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
                    char threadName[gThreadNameMaxLength] = {0};
                    ::snprintf(threadName, gThreadNameMaxLength, "%s-Thread-%u", name_.c_str(), numOfActiveThreads(ctl_));
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