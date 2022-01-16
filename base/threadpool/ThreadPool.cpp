//
// Created by guang19 on 2021/12/30.
//

#include "base/threadpool/ThreadPool.h"

namespace nets
{
    namespace base
    {
        ThreadPool::ThreadWrapper::ThreadWrapper(uint32_t id, ThreadPoolRawPtr threadPoolPtr)
            : ThreadWrapper(id, nullptr, threadPoolPtr)
        {}

        ThreadPool::ThreadWrapper::ThreadWrapper(uint32_t id, const task_type& task, ThreadPoolRawPtr threadPoolPtr)
            : id_(id), task_(task), thread_(&ThreadWrapper::startThread, this), threadPoolPtr_(threadPoolPtr)
        {}

        void ThreadPool::ThreadWrapper::startThread()
        {
            threadPoolPtr_->runThread(this);
        }

        ThreadPool::ThreadPool(uint32_t corePoolSize, uint32_t maxPoolSize)
            : ThreadPool(defaultThreadPoolName_, corePoolSize, maxPoolSize, defaultKeepAliveTime_,
                         defaultMaxQueueSize_)
        {}

        ThreadPool::ThreadPool(uint32_t corePoolSize, uint32_t maxPoolSize, const milliseconds_type& keepAliveTime)
            : ThreadPool(defaultThreadPoolName_, corePoolSize, maxPoolSize, keepAliveTime,
                         defaultMaxQueueSize_)
        {}

        ThreadPool::ThreadPool(uint32_t corePoolSize, uint32_t maxPoolSize, const milliseconds_type& keepAliveTime,
                               size_type maxQueueSize)
                : ThreadPool(defaultThreadPoolName_, corePoolSize, maxPoolSize, keepAliveTime,
                             maxQueueSize)
        {}

        ThreadPool::ThreadPool(const std::string& name, uint32_t corePoolSize, uint32_t maxPoolSize)
            : ThreadPool(name, corePoolSize, maxPoolSize, defaultKeepAliveTime_,
                         defaultMaxQueueSize_)
        {}

        ThreadPool::ThreadPool(const std::string& name, uint32_t corePoolSize, uint32_t maxPoolSize,
                   const milliseconds_type& keepAliveTime)
                   : ThreadPool(name, corePoolSize, maxPoolSize, keepAliveTime, defaultMaxQueueSize_)
        {}
        ThreadPool::ThreadPool(const std::string& name, uint32_t corePoolSize, uint32_t maxPoolSize,
                               const milliseconds_type& keepAliveTime, size_type maxQueueSize)
                   : name_(name), running_(false), corePoolSize_(corePoolSize), maxPoolSize_(maxPoolSize),
                        keepAliveTime_(keepAliveTime)
        {
            blockingQueuePtr_ = BlockingQueuePtr(new BlockingQueue<task_type>(maxQueueSize));
        }

        void ThreadPool::runThread(ThreadWrapperRawPtr threadWrapper)
        {
            while (running_)
            {
                blockingQueuePtr_->take(threadWrapper->getTaskRef());
                if (threadWrapper->getTaskRef())
                {
                    threadWrapper->getTaskRef()();
                }
            }
        }
    } // namespace base
} //namespace nets