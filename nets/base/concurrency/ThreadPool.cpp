//
// Created by guang19 on 2021/12/30.
//

#include <utility>

#include "nets/base/concurrency/ThreadPool.h"

namespace nets
{
    namespace base
    {
		namespace
		{
			constexpr const char* DefaultThreadPoolName = "NetsThreadPool";
			constexpr ::std::time_t DefaultKeepAliveTime = 30000;
			constexpr ThreadPool::SizeType DefaultMaxQueueSize = INT32_MAX;
			constexpr enum ThreadPool::RejectionPolicy DefaultRejectionPolicy =
				ThreadPool::RejectionPolicy::DiscardOlderPolicy;
		}

        ThreadPool::ThreadWrapper::ThreadWrapper(SizeType id, bool isCoreThread, ThreadPoolRawPtr threadPoolRawPtr)
			: ThreadWrapper(id, false, nullptr, threadPoolRawPtr)
        {
		}

        ThreadPool::ThreadWrapper::ThreadWrapper(SizeType id, bool isCoreThread, TaskType task, ThreadPoolRawPtr threadPoolRawPtr)
            : id_(id), isCoreThread_(isCoreThread), task_(std::move(task)), thread_(::std::bind(&ThreadWrapper::startThread, this)),
			threadPoolRawPtr_(threadPoolRawPtr)
        {
			thread_.start();
        }

        void ThreadPool::ThreadWrapper::startThread()
        {
			threadPoolRawPtr_->runThread(this);
        }

        ThreadPool::ThreadPool(SizeType corePoolSize, SizeType maximumPoolSize) :
			ThreadPool(DefaultThreadPoolName, corePoolSize, maximumPoolSize, DefaultKeepAliveTime,
						 DefaultMaxQueueSize, DefaultRejectionPolicy)
        {
		}

        ThreadPool::ThreadPool(SizeType corePoolSize, SizeType maximumPoolSize, ::std::time_t keepAliveTime) :
			ThreadPool(DefaultThreadPoolName, corePoolSize, maximumPoolSize, keepAliveTime,
						 DefaultMaxQueueSize, DefaultRejectionPolicy)
        {
		}

        ThreadPool::ThreadPool(SizeType corePoolSize, SizeType maximumPoolSize, ::std::time_t keepAliveTime,
			SizeType maxQueueSize) : ThreadPool(DefaultThreadPoolName, corePoolSize, maximumPoolSize,
			keepAliveTime, maxQueueSize, DefaultRejectionPolicy)
		{
		}

        ThreadPool::ThreadPool(const ::std::string& name, SizeType corePoolSize, SizeType maximumPoolSize,
			enum RejectionPolicy rejectionPolicy) : ThreadPool(name, corePoolSize, maximumPoolSize,
			DefaultKeepAliveTime, DefaultMaxQueueSize, rejectionPolicy)
        {
		}

        ThreadPool::ThreadPool(const ::std::string& name, SizeType corePoolSize, SizeType maximumPoolSize,
			::std::time_t keepAliveTime, enum RejectionPolicy rejectionPolicy) : ThreadPool(name, corePoolSize,
			maximumPoolSize, keepAliveTime, DefaultMaxQueueSize, rejectionPolicy)
        {
		}

		ThreadPool::ThreadPool(const ::std::string& name, SizeType corePoolSize, SizeType maximumPoolSize,
							   ::std::time_t keepAliveTime, SizeType maxQueueSize, enum RejectionPolicy rejectionPolicy)
			: name_(name), running_(false), corePoolSize_(corePoolSize), maximumPoolSize_(maximumPoolSize),
			  keepAliveTime_(keepAliveTime), taskQueue_(new BoundedBlockingQueue<TaskType>(maxQueueSize)),
			  rejectionPolicy_(rejectionPolicy)

		{
			if (corePoolSize <= 0 || corePoolSize > maximumPoolSize)
			{
				LOG_FATAL("corePoolSize must be greater than 0 and maximumPoolSize must be greater than corePoolSize");
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
				LOG_WARN("thread pool has been initialized");
				return;
			}
			running_ = true;
			threadPool_.reserve(corePoolSize_);
			LOG_DEBUG("thread pool init success");
		}

		void ThreadPool::shutdown()
		{
			if (!running_)
			{
				return;
			}
			running_ = false;
			LockGuardType lock(mutex_);
			// notify blocking thread
			taskQueue_->notifyBlockingThread();
			while (!threadPool_.empty())
			{
				poolCV_.wait(mutex_);
			}
			LOG_DEBUG("thread pool has been shutdown");
		}

        void ThreadPool::runThread(ThreadWrapperRawPtr threadWrapperRawPtr)
        {
			if (threadWrapperRawPtr == nullptr)
			{
				return;
			}
			if (threadWrapperRawPtr->task_ != nullptr)
			{
				threadWrapperRawPtr->task_();
				threadWrapperRawPtr->task_ = nullptr;
			}
			::std::function<bool ()> notRunning = [&]() -> bool
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
					if (!taskQueue_->take(threadWrapperRawPtr->task_, keepAliveTime_, notRunning))
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
					poolCV_.notifyAll();
					break;
				}
			}
		}

		bool ThreadPool::addThreadTask(const TaskType& task, bool isCore, SizeType currentThreadSize)
		{
			threadPool_.emplace_back(new ThreadWrapper(currentThreadSize, isCore, task,
				this));
			return true;
		}

		bool ThreadPool::rejectedExecution(const TaskType& task)
		{
			switch (rejectionPolicy_)
			{
				case RejectionPolicy::DiscardPolicy:
					return false;
				case RejectionPolicy::DiscardOlderPolicy:
					taskQueue_->popBack();
					return taskQueue_->tryPush(task);
				case RejectionPolicy::CallerRunsPolicy:
					task();
					return true;
				default:
					return false;
			}
		}
	} // namespace base
} // namespace nets