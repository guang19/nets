//
// Created by guang19 on 2021/12/30.
//

#include "nets/base/concurrency/ThreadPool.h"

#include <iostream>

namespace nets
{
    namespace base
    {
		namespace
		{
			constexpr const char* DefaultThreadPoolName { "NetsThreadPool" };
			constexpr ThreadPool::milliseconds_type DefaultKeepAliveTime { 30000 };
			constexpr ThreadPool::size_type DefaultMaxQueueSize { INT32_MAX };
		}

        ThreadPool::ThreadWrapper::ThreadWrapper(size_type id, bool isCoreThread, ThreadPoolRawPtr threadPoolRawPtr)
			: ThreadWrapper(id, false, nullptr, threadPoolRawPtr)
        {}

        ThreadPool::ThreadWrapper::ThreadWrapper(size_type id, bool isCoreThread, const task_type& task, ThreadPoolRawPtr threadPoolRawPtr)
            : id_(id), isCoreThread_(isCoreThread), task_(task), thread_(&ThreadWrapper::startThread, this),
			threadPoolRawPtr_(threadPoolRawPtr)
        {
            if (thread_.joinable())
            {
                thread_.detach();
            }
        }

        void ThreadPool::ThreadWrapper::startThread()
        {
			threadPoolRawPtr_->runThread(this);
        }

        ThreadPool::ThreadPool(size_type corePoolSize, size_type maximumPoolSize)
			: ThreadPool(DefaultThreadPoolName, corePoolSize, maximumPoolSize, DefaultKeepAliveTime,
						 DefaultMaxQueueSize)
        {}

        ThreadPool::ThreadPool(size_type corePoolSize, size_type maximumPoolSize, const milliseconds_type& keepAliveTime)
            : ThreadPool(DefaultThreadPoolName, corePoolSize, maximumPoolSize, keepAliveTime,
						 DefaultMaxQueueSize)
        {}

        ThreadPool::ThreadPool(size_type corePoolSize, size_type maximumPoolSize, const milliseconds_type& keepAliveTime,
							   size_type maxQueueSize)
							   : ThreadPool(DefaultThreadPoolName, corePoolSize, maximumPoolSize, keepAliveTime,
											maxQueueSize)
        {}

        ThreadPool::ThreadPool(const ::std::string& name, size_type corePoolSize, size_type maximumPoolSize)
			: ThreadPool(name, corePoolSize, maximumPoolSize, DefaultKeepAliveTime,
						 DefaultMaxQueueSize)
        {}

        ThreadPool::ThreadPool(const ::std::string& name, size_type corePoolSize, size_type maximumPoolSize,
							   const milliseconds_type& keepAliveTime)
							   : ThreadPool(name, corePoolSize, maximumPoolSize, keepAliveTime, DefaultMaxQueueSize)
        {}

        ThreadPool::ThreadPool(const ::std::string& name, size_type corePoolSize, size_type maximumPoolSize,
                               const milliseconds_type& keepAliveTime, size_type maxQueueSize)
							   : name_(name), running_(false), corePoolSize_(corePoolSize), maximumPoolSize_(maximumPoolSize),
							   keepAliveTime_(keepAliveTime), taskQueue_(new BoundedBlockingQueue<task_type>(maxQueueSize)),
							   rejectionPolicy_(RejectionPolicy::DiscardPolicy)

		{
			if (corePoolSize <= 0 || corePoolSize > maximumPoolSize)
			{
				throw ::std::invalid_argument("corePoolSize must be greater than 0 and maximumPoolSize must be greater than corePoolSize");
			}
		}

		ThreadPool::~ThreadPool()
		{
			shutdown();
		}

		void ThreadPool::init()
		{
			if (running_)
			{
				return;
			}
			running_ = true;
		}

		void ThreadPool::shutdown()
		{
			if (running_)
			{
				running_ = false;
				unique_lock_type lock(mtx_);
				// notify blocking thread
				taskQueue_->notifyBlockingThread();
				poolCv_.wait(lock, [&]()
				{
					return pool_.empty();
				});
            }
		}

        void ThreadPool::runThread(ThreadWrapperRawPtr threadWrapperRawPtr)
        {
			if (threadWrapperRawPtr == nullptr)
			{
				return;
			}
			if (threadWrapperRawPtr->task_)
			{
				threadWrapperRawPtr->task_();
				threadWrapperRawPtr->task_ = nullptr;
			}
            while (running_)
            {
				// core thread blocked wait
				if (threadWrapperRawPtr->isCoreThread_)
				{
					taskQueue_->take(threadWrapperRawPtr->task_, [&]
					{
						return !isRunning();
					});
				}
				else
				{
					// non-core thread wait timeout
					if (!taskQueue_->take(threadWrapperRawPtr->task_, keepAliveTime_, [&]
					{
						return !isRunning();
					}))
					{
						break;
					}
				}
                if (threadWrapperRawPtr->task_)
                {
					threadWrapperRawPtr->task_();
					threadWrapperRawPtr->task_ = nullptr;
                }
            }
			releaseThread(threadWrapperRawPtr);
        }

		void ThreadPool::releaseThread(ThreadWrapperRawPtr threadWrapperRawPtr)
		{
			lock_guard_type lock(mtx_);
			for (auto it = pool_.begin(), end = pool_.end(); it != end; ++it)
			{
				if (it->get() == threadWrapperRawPtr)
				{
					pool_.erase(it);
					poolCv_.notify_all();
					break;
				}
			}
		}

		bool ThreadPool::addThreadTask(const task_type& task, bool isCore, size_type currentThreadSize)
		{
			if (running_)
			{
				pool_.emplace_back(new ThreadWrapper(currentThreadSize + 1, isCore, task, this));
				return true;
			}
			return false;
		}

		bool ThreadPool::rejectedExecution(const ThreadPool::task_type &task)
		{
			switch (rejectionPolicy_)
			{
				case RejectionPolicy::DiscardPolicy:
					return false;
				case RejectionPolicy::DiscardOlderPolicy:
				{
					taskQueue_->pop_back();
					return taskQueue_->tryPush(task);
				}
				case RejectionPolicy::CallerRunsPolicy:
				{
					task();
					return true;
				}
			}
			return false;
		}
	} // namespace base
} // namespace nets