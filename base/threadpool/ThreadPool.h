//
// Created by guang19 on 2021/12/30.
//

#ifndef NETS_BASE_THREAD_POOL_H
#define NETS_BASE_THREAD_POOL_H

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <vector>
#include "base/noncopyable.h"
#include "base/threadpool/BlockingQueue.h"

namespace nets
{
    namespace base
    {
        class ThreadPool : noncopyable
        {
            public:
                using size_type                 = std::size_t;
                using atomic_bool_type          = std::atomic<bool>;
                using task_type                 = std::function<void()>;
                using milliseconds_type         = std::chrono::duration<std::size_t, std::milli>;
                using condition_variable_type   = std::condition_variable;
                using ThreadPoolRawPtr          = ThreadPool*;
                using BlockingQueuePtr          = std::unique_ptr<BlockingQueue<task_type>>;

            private:
                class ThreadWrapper : noncopyable
                {
                    public:
                        ThreadWrapper(uint32_t id, ThreadPoolRawPtr threadPoolPtr);
                        ThreadWrapper(uint32_t id, const task_type& task, ThreadPoolRawPtr threadPoolPtr);

                        void setTask(const task_type& task)
                        {
                            task_ = task;
                        }

                        task_type& getTaskRef()
                        {
                            return task_;
                        }

                    private:
                        void startThread();

                    private:
                        uint32_t id_;
                        // is core thread
                        // bool isCoreThread_;
                        task_type task_;
                        std::thread thread_;
                        ThreadPoolRawPtr threadPoolPtr_;
                };

            public:
                ThreadPool(uint32_t corePoolSize, uint32_t maxPoolSize);
                ThreadPool(uint32_t corePoolSize, uint32_t maxPoolSize, const milliseconds_type& keepAliveTime);
                ThreadPool(uint32_t corePoolSize, uint32_t maxPoolSize, const milliseconds_type& keepAliveTime,
                           size_type maxQueueSize);
                ThreadPool(const std::string& name, uint32_t corePoolSize, uint32_t maxPoolSize);
                ThreadPool(const std::string& name, uint32_t corePoolSize, uint32_t maxPoolSize,
                           const milliseconds_type& keepAliveTime);
                ThreadPool(const std::string& name, uint32_t corePoolSize, uint32_t maxPoolSize,
                           const milliseconds_type& keepAliveTime, size_type maxQueueSize);

                void init();
                void shutdown();

                bool running() const
                {
                    return running_;
                }

                const std::string& name() const
                {
                    return name_;
                }

            private:
                using ThreadWrapperRawPtr   = ThreadWrapper*;
                using ThreadWrapperPtr      = std::unique_ptr<ThreadWrapper>;
                void runThread(ThreadWrapperRawPtr threadWrapper);

            private:
                std::string name_;
                atomic_bool_type running_;
                uint32_t corePoolSize_;
                uint32_t maxPoolSize_;
                milliseconds_type keepAliveTime_;
                std::vector<ThreadWrapperPtr> pool_;
                BlockingQueuePtr blockingQueuePtr_;
                std::mutex mtx_;
                condition_variable_type cv_;

                static const std::string& defaultThreadPoolName_;
                static const milliseconds_type& defaultKeepAliveTime_;
                static const size_type defaultMaxQueueSize_;
        };
        const std::string& ThreadPool::defaultThreadPoolName_ = "NetsThreadPool";
        const ThreadPool::milliseconds_type& ThreadPool::defaultKeepAliveTime_ = milliseconds_type(30000);
        const ThreadPool::size_type ThreadPool::defaultMaxQueueSize_ = UINT32_MAX;
    } // namespace base
} // namespace nets

#endif // NETS_BASE_THREAD_POOL_H
