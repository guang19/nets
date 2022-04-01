//
// Created by guang19 on 2021/12/30.
//

#include "base/threadpool/ThreadPool.h"
#include <iostream>
#include <type_traits>

namespace nets
{
    namespace base
    {
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
				// 无效的核心线程数和最大线程数
				throw ::std::invalid_argument("corePoolSize must be greater than 0 and maximumPoolSize must be greater than corePoolSize");
			}
		}

		ThreadPool::~ThreadPool()
		{
			shutdown();
			std::cout << "任务：" << taskQueue_->size() << std::endl;
			std::cout << "线程：" << pool_.size() << std::endl;
		}

		void ThreadPool::init()
		{
			if (running_)
			{
				// 正在运行或者已经初始化了
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
				// 唤醒阻塞在任务队列的线程
				taskQueue_->notifyBlockingThread();
				// 还有线程在运行就等待
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
				// 核心线程可以阻塞等待
				if (threadWrapperRawPtr->isCoreThread_)
				{
					taskQueue_->take(threadWrapperRawPtr->task_, [&]
					{
						return !isRunning();
					});
				}
				else
				{
					// 非核心线程则等待超时
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
		}
	} // namespace base
} // namespace nets