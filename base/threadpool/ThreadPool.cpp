//
// Created by guang19 on 2021/12/30.
//

#include "base/threadpool/ThreadPool.h"

namespace nets
{
    namespace base
    {
        ThreadPool::ThreadWrapper::ThreadWrapper(uint32_t id, bool isCoreThread, ThreadPoolRawPtr threadPoolRawPtr)
			: ThreadWrapper(id, false, nullptr, threadPoolRawPtr)
        {}

        ThreadPool::ThreadWrapper::ThreadWrapper(uint32_t id, bool isCoreThread, const task_type& task, ThreadPoolRawPtr threadPoolRawPtr)
            : id_(id), isCoreThread_(isCoreThread), task_(task), thread_(&ThreadWrapper::startThread, this), threadPoolRawPtr_(threadPoolRawPtr)
        {}

        void ThreadPool::ThreadWrapper::startThread()
        {
			threadPoolRawPtr_->runThread(this);
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
							   keepAliveTime_(keepAliveTime), blockingQueuePtr_(new BlockingQueue<task_type>(maxQueueSize))
		{
			if (corePoolSize <= 0 || corePoolSize > maxPoolSize)
			{
				// 无效的核心线程数和最大线程数
				return;
			}
		}

		ThreadPool::~ThreadPool()
		{
			shutdown();
		}

		void ThreadPool::init()
		{
			lock_guard_type lock(mtx_);
			if (running_)
			{
				// 正在运行已经初始化了
				return;
			}
			running_ = true;
			pool_.reserve(corePoolSize_);
			for (uint32_t i = 0; i < corePoolSize_; ++i)
			{
				pool_.emplace_back(new ThreadWrapper(i + 1, true, this));
			}
		}

		void ThreadPool::shutdown()
		{
			lock_guard_type lock(mtx_);
			if (running_)
			{
				running_ = false;
			}
		}

        void ThreadPool::runThread(ThreadWrapperRawPtr threadWrapperRawPtr)
        {
			if (threadWrapperRawPtr == nullptr)
			{
				return;
			}
            while (running_)
            {
				if (threadWrapperRawPtr->getTask())
				{
					threadWrapperRawPtr->getTask()();
					threadWrapperRawPtr->setTask(nullptr);
				}
				if (threadWrapperRawPtr->coreThread())
				{
					blockingQueuePtr_->take(threadWrapperRawPtr->getTask());
				}
				else
				{
					blockingQueuePtr_->take(threadWrapperRawPtr->getTask(), keepAliveTime_);
				}
                if (threadWrapperRawPtr->getTask())
                {
					threadWrapperRawPtr->getTask()();
					threadWrapperRawPtr->setTask(nullptr);
                }
            }
			lock_guard_type lock(mtx_);
			for (auto it = pool_.begin(), end = pool_.end(); it != end; ++it)
			{
				if (it->get() == threadWrapperRawPtr)
				{
					pool_.erase(it);
					break;
				}
			}
        }

		void ThreadPool::execute(const task_type& task)
		{
			if (!running_)
			{
				// 没有运行不接受任务
				return;
			}
			uint32_t threadSize = pool_.size();
			if (threadSize < maxPoolSize_)
			{
				pool_.emplace_back(new ThreadWrapper(threadSize + 1, false, task, this));
			}
			else
			{
				blockingQueuePtr_->tryPush(task);
			}
		}
    } // namespace base
} //namespace nets