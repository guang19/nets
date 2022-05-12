//
// Created by guang19 on 2021/12/30.
//

#include "nets/base/concurrency/ThreadPool.h"

#include <unistd.h>
#include <utility>

#include "nets/base/CommonMacro.h"

#ifndef CORE_POOL_SIZE
#define CORE_POOL_SIZE (AVAILABLE_PROCESSOR << 1)
#endif

#ifndef MAX_POOL_SIZE
#define MAX_POOL_SIZE CORE_POOL_SIZE
#endif

#ifndef IDLE_KEEP_ALIVE_TIME
#define IDLE_KEEP_ALIVE_TIME 30000
#endif

#ifndef TASK_QUEUE_SIZE
#define TASK_QUEUE_SIZE 24
#endif

namespace nets::base
{
	namespace
	{
		constexpr const char* const DefaultThreadPoolName = "ThreadPool";
		constexpr enum ThreadPool::RejectionPolicy DefaultRejectionPolicy = ThreadPool::RejectionPolicy::DiscardOlderPolicy;
	} // namespace

	ThreadPool::ThreadWrapper::ThreadWrapper(const char* threadName, bool isCoreThread, TaskType task,
											 ThreadPoolPtr threadPoolPtr)
		: threadName_(threadName), isCoreThread_(isCoreThread), task_(std::move(task)),
		  thread_(&ThreadWrapper::start, this), threadPoolPtr_(threadPoolPtr)
	{
	}

	ThreadPool::ThreadWrapper::~ThreadWrapper()
	{
		if (thread_.joinable())
		{
			thread_.detach();
		}
	}

	void ThreadPool::ThreadWrapper::start()
	{
		setCurrentThreadName(threadName_.c_str());
		threadPoolPtr_->runThread(this);
	}

	ThreadPool::ThreadPool()
		: ThreadPool(DefaultThreadPoolName, CORE_POOL_SIZE, MAX_POOL_SIZE, IDLE_KEEP_ALIVE_TIME, TASK_QUEUE_SIZE,
					 DefaultRejectionPolicy)
	{
	}

	ThreadPool::ThreadPool(SizeType corePoolSize, SizeType maxPoolSize)
		: ThreadPool(DefaultThreadPoolName, corePoolSize, maxPoolSize, IDLE_KEEP_ALIVE_TIME, TASK_QUEUE_SIZE,
					 DefaultRejectionPolicy)
	{
	}

	ThreadPool::ThreadPool(SizeType corePoolSize, SizeType maxPoolSize, TimeType keepAliveTime)
		: ThreadPool(DefaultThreadPoolName, corePoolSize, maxPoolSize, keepAliveTime, TASK_QUEUE_SIZE,
					 DefaultRejectionPolicy)
	{
	}

	ThreadPool::ThreadPool(SizeType corePoolSize, SizeType maxPoolSize, TimeType keepAliveTime, SizeType maxQueueSize)
		: ThreadPool(DefaultThreadPoolName, corePoolSize, maxPoolSize, keepAliveTime, maxQueueSize, DefaultRejectionPolicy)
	{
	}

	ThreadPool::ThreadPool(const ::std::string& name, SizeType corePoolSize, SizeType maxPoolSize,
						   enum RejectionPolicy rejectionPolicy)
		: ThreadPool(name, corePoolSize, maxPoolSize, IDLE_KEEP_ALIVE_TIME, TASK_QUEUE_SIZE, rejectionPolicy)
	{
	}

	ThreadPool::ThreadPool(const ::std::string& name, SizeType corePoolSize, SizeType maxPoolSize, TimeType keepAliveTime,
						   enum RejectionPolicy rejectionPolicy)
		: ThreadPool(name, corePoolSize, maxPoolSize, keepAliveTime, TASK_QUEUE_SIZE, rejectionPolicy)
	{
	}

	ThreadPool::ThreadPool(const ::std::string& name, SizeType corePoolSize, SizeType maxPoolSize,
						   TimeType idleKeepAliveTime, SizeType maxQueueSize, enum RejectionPolicy rejectionPolicy)
		: name_(name), running_(false), corePoolSize_(corePoolSize), maxPoolSize_(maxPoolSize),
		  idleKeepAliveTime_(idleKeepAliveTime), taskQueue_(::std::make_unique<BlockingQueueType>(maxQueueSize)),
		  rejectionPolicy_(rejectionPolicy)

	{
		if (corePoolSize_ <= 0 || corePoolSize_ > maxPoolSize_)
		{
			LOGS_FATAL << "corePoolSize must be greater than 0 and maxPoolSize must be greater than maxPoolSize";
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
			LOGS_WARN << "thread pool has been initialized";
			return;
		}
		running_ = true;
		threadPool_.reserve(maxPoolSize_);
		LOGS_DEBUG << "thread pool init success";
	}

	void ThreadPool::shutdown()
	{
		if (!running_)
		{
			return;
		}
		running_ = false;
		UniqueLockType lock(mutex_);
		// notify blocking thread
		taskQueue_->notifyBlockingThread();
		poolCV_.wait(lock,
					 [this]() -> bool
					 {
						 return threadPool_.empty();
					 });
		assert(taskQueue_->isEmpty());
		assert(threadPool_.empty());
		LOGS_DEBUG << "thread pool has been shutdown";
	}

	void ThreadPool::runThread(ThreadWrapperRawPtr threadWrapperRawPtr)
	{
		if (threadWrapperRawPtr->task_ != nullptr)
		{
			threadWrapperRawPtr->task_();
			threadWrapperRawPtr->task_ = nullptr;
		}
		::std::function<bool()> notRunning = [this]() -> bool
		{
			return !isRunning();
		};
		while (running_)
		{
			// core thread blocked wait
			if (threadWrapperRawPtr->isCoreThread_)
			{
				taskQueue_->take(threadWrapperRawPtr->task_, notRunning);
			}
			else
			{
				// non-core thread wait timeout
				if (!taskQueue_->take(threadWrapperRawPtr->task_, idleKeepAliveTime_, notRunning))
				{
					break;
				}
			}
			if (threadWrapperRawPtr->task_ != nullptr)
			{
				threadWrapperRawPtr->task_();
				threadWrapperRawPtr->task_ = nullptr;
			}
		}
		releaseThread(threadWrapperRawPtr);
	}

	void ThreadPool::releaseThread(ThreadWrapperRawPtr threadWrapperRawPtr)
	{
		LockGuardType lock(mutex_);
		for (auto it = threadPool_.begin(), end = threadPool_.end(); it != end; ++it)
		{
			if (it->get() == threadWrapperRawPtr)
			{
				threadPool_.erase(it);
				poolCV_.notify_all();
				break;
			}
		}
	}

	bool ThreadPool::addThreadTask(const TaskType& task, bool isCore, SizeType threadSize)
	{
		++threadSize;
		char threadName[ThreadNameMaxLength] = {0};
		::snprintf(threadName, ThreadNameMaxLength, "Thread-%zu", threadSize);
		threadPool_.emplace_back(new ThreadWrapper(threadName, isCore, task, this));
		return true;
	}

	void ThreadPool::reject(const TaskType& task)
	{
		switch (rejectionPolicy_)
		{
			case RejectionPolicy::DiscardPolicy:
				break;
			case RejectionPolicy::DiscardOlderPolicy:
			{
				taskQueue_->popFront();
				submit(task);
				break;
			}
			case RejectionPolicy::CallerRunsPolicy:
			{
				task();
			}
		}
	}
} // namespace nets::base