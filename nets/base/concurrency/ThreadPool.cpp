//
// Created by guang19 on 2021/12/30.
//

#include "nets/base/concurrency/ThreadPool.h"

#include <utility>

#include "nets/base/CommonMacro.h"

namespace nets::base
{
	const ThreadPool::SizeType ThreadPool::DefaultCorePoolSize = AVAILABLE_PROCESSOR << 1;
	const ThreadPool::SizeType ThreadPool::DefaultMaxPoolSize = DefaultCorePoolSize;

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

	ThreadPool::ThreadPool(SizeType corePoolSize, SizeType maximumPoolSize, BlockingQueueType::SizeType maxQueueSize,
						   TimeType idleKeepAliveTime, const ::std::string& name)
		: corePoolSize_(corePoolSize), maximumPoolSize_(maximumPoolSize), idleKeepAliveTime_(idleKeepAliveTime),
		  taskQueue_(::std::make_unique<BlockingQueueType>(maxQueueSize)), threadPool_(), name_(name), ctl_(Running),
		  mutex_(), poolCV_()
	{
		if (corePoolSize_ <= 0 || corePoolSize_ > maximumPoolSize_)
		{
			LOGS_FATAL << "corePoolSize must be greater than 0 and maxPoolSize must be greater than maxPoolSize";
		}
		threadPool_.reserve(maximumPoolSize_);
		LOGS_INFO << "threadpool [" << name_ << "] init success";
	}

	ThreadPool::~ThreadPool()
	{
		shutdown();
	}

	void ThreadPool::shutdown()
	{
		if (isShutdown())
		{
			LOGS_DEBUG << "threadpool [" << name_ << "] has been shutdown";
			return;
		}
		// cas
		while (true)
		{
			SizeType ctl = ctl_.load();
			// set state to shutdown
			if (ctl_.compare_exchange_strong(ctl, (Shutdown | (ctl & Capacity))))
			{
				break;
			}
		}
		// notify blocking thread
		taskQueue_->notifyBlockingThread();
		UniqueLockType lock(mutex_);
		poolCV_.wait(lock,
					 [this]() -> bool
					 {
						 return numOfActiveThreads() == 0;
					 });
		assert(threadPool_.empty());
		// if threadpool has no thread takes task from task queue, it needs to be deleted manually
		TaskType tmpTask = nullptr;
		while (!taskQueue_->isEmpty())
		{
			taskQueue_->tryPop(tmpTask);
		}
		assert(taskQueue_->isEmpty());
		LOGS_INFO << "threadpool [" << name_ << "] shutdown success";
	}

	void ThreadPool::runThread(ThreadWrapperRawPtr threadWrapperRawPtr)
	{
		if (threadWrapperRawPtr->task_ != nullptr)
		{
			threadWrapperRawPtr->task_();
			threadWrapperRawPtr->task_ = nullptr;
		}
		::std::function<bool()> shutdown = [this]() -> bool
		{
			return isShutdown();
		};
		while (isRunning())
		{
			// core thread blocked wait
			if (threadWrapperRawPtr->isCoreThread_)
			{
				taskQueue_->take(threadWrapperRawPtr->task_, shutdown);
			}
			else
			{
				// non-core thread wait timeout
				if (!taskQueue_->take(threadWrapperRawPtr->task_, idleKeepAliveTime_, shutdown))
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
				--ctl_;
				poolCV_.notify_all();
				break;
			}
		}
	}

	bool ThreadPool::addThreadTask(const TaskType& task, bool isCore)
	{
		// cas
		while (true)
		{
			SizeType ctl = ctl_.load();
			if (isShutdown(ctl))
			{
				return false;
			}
			SizeType n = numOfActiveThreads(ctl);
			if (n >= (isCore ? corePoolSize_ : maximumPoolSize_))
			{
				return false;
			}
			if (ctl_.compare_exchange_strong(ctl, ctl + 1))
			{
				LockGuardType lock(mutex_);
				if (isRunning())
				{
					char threadName[ThreadNameMaxLength] = {0};
					::snprintf(threadName, ThreadNameMaxLength, "%s-Thread-%lu", name_.c_str(), numOfActiveThreads(ctl + 1));
					threadPool_.emplace_back(new ThreadWrapper(threadName, isCore, task, this));
					return true;
				}
				else
				{
					--ctl_;
					poolCV_.notify_all();
					return false;
				}
			}
		}
	}
} // namespace nets::base