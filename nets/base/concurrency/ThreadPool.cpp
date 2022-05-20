//
// Created by guang19 on 2021/12/30.
//

#include "nets/base/concurrency/ThreadPool.h"

#include <utility>

namespace nets::base
{
	ThreadPool::ThreadWrapper::ThreadWrapper(const char* threadName, bool isCoreThread, TaskType task,
											 ThreadPoolPtr threadPoolPtr)
		: threadName_(threadName), isCoreThread_(isCoreThread), task_(std::move(task)), thread_(&ThreadWrapper::start, this),
		  threadPoolPtr_(threadPoolPtr)
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

	ThreadPool::ThreadPool(SizeType corePoolSize, SizeType maxPoolSize, TimeType idleKeepAliveTime, SizeType maxQueueSize,
						   enum RejectionPolicy rejectionPolicy, const ::std::string& name)
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
			LOGS_DEBUG << "thread pool has been initialized";
			return;
		}
		running_ = true;
		threadPool_.reserve(maxPoolSize_);
		LOGS_INFO << "thread pool init success";
	}

	void ThreadPool::shutdown()
	{
		if (!running_)
		{
			LOGS_DEBUG << "thread pool has been shutdown";
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
		LOGS_INFO << "thread pool shutdown success";
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