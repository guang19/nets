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
			constexpr ::time_t DefaultKeepAliveTime = 30000;
			constexpr ThreadPool::SizeType DefaultMaxQueueSize = INT32_MAX;
			constexpr enum ThreadPool::RejectionPolicy DefaultRejectionPolicy =
				ThreadPool::RejectionPolicy::DiscardOlderPolicy;
		}

        ThreadPool::ThreadWrapper::ThreadWrapper(const ::std::string& name, bool isCoreThread,
			ThreadPoolRawPtr threadPoolRawPtr) : ThreadWrapper(name, false, nullptr, threadPoolRawPtr)
        {
		}

        ThreadPool::ThreadWrapper::ThreadWrapper(const ::std::string& name, bool isCoreThread, TaskType task,
			ThreadPoolRawPtr threadPoolRawPtr) : isCoreThread_(isCoreThread), task_(std::move(task)),
			thread_(::std::bind(&ThreadWrapper::startThread, this), name),
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

        ThreadPool::ThreadPool(SizeType corePoolSize, SizeType maximumPoolSize, ::time_t keepAliveTime) :
			ThreadPool(DefaultThreadPoolName, corePoolSize, maximumPoolSize, keepAliveTime,
						 DefaultMaxQueueSize, DefaultRejectionPolicy)
        {
		}

        ThreadPool::ThreadPool(SizeType corePoolSize, SizeType maximumPoolSize, ::time_t keepAliveTime,
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
			::time_t keepAliveTime, enum RejectionPolicy rejectionPolicy) : ThreadPool(name, corePoolSize,
			maximumPoolSize, keepAliveTime, DefaultMaxQueueSize, rejectionPolicy)
        {
		}

		ThreadPool::ThreadPool(const ::std::string& name, SizeType corePoolSize, SizeType maximumPoolSize,
							   ::time_t keepAliveTime, SizeType maxQueueSize, enum RejectionPolicy rejectionPolicy)
			: name_(name), running_(false), corePoolSize_(corePoolSize), maximumPoolSize_(maximumPoolSize),
			  keepAliveTime_(keepAliveTime), taskQueue_(new BoundedBlockingQueue<TaskType>(maxQueueSize)),
			  rejectionPolicy_(rejectionPolicy)

		{
			if (corePoolSize <= 0 || corePoolSize > maximumPoolSize)
			{
				LOGS_FATAL << "corePoolSize must be greater than 0 and maximumPoolSize must be greater than corePoolSize";
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
			threadPool_.reserve(corePoolSize_);
			LOGS_DEBUG << "thread pool init success";
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
			LOGS_DEBUG << "thread pool has been shutdown";
		}

        void ThreadPool::runThread(ThreadWrapperRawPtr threadWrapperRawPtr)
        {
			if (threadWrapperRawPtr->task_ != nullptr)
			{
				try
				{
					threadWrapperRawPtr->task_();
				}
				catch (const ::std::exception& exception)
				{
					LOGS_ERROR << "exception caught during thread [" << threadWrapperRawPtr->thread_.getThreadName()
						<< "] execution in thread pool [" << name_ << "], reason " << exception.what();
				}
				catch(...)
				{
					LOGS_ERROR << "unknown exception caught during thread [" <<
						threadWrapperRawPtr->thread_.getThreadName() << "] execution in thread pool [" << name_ << ']';
				}
				threadWrapperRawPtr->task_ = nullptr;
			}
			::std::function<bool ()> notRunning = [&]() -> bool{
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
					try
					{
						threadWrapperRawPtr->task_();
					}
					catch (const ::std::exception& exception)
					{
						LOGS_ERROR << "exception caught during thread [" << threadWrapperRawPtr->thread_.getThreadName()
							<< "] execution in thread pool [" << name_ << "], reason " << exception.what();
					}
					catch(...)
					{
						LOGS_ERROR << "unknown exception caught during thread [" <<
						threadWrapperRawPtr->thread_.getThreadName() << "] execution in thread pool [" << name_ << ']';
					}
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

		bool ThreadPool::addThreadTask(const TaskType& task, bool isCore, SizeType threadSize)
		{
			threadSize += 1;
			char name[24] = { 0 };
			::snprintf(name, 24, "NesThreadPool-%lu", threadSize);
			threadPool_.emplace_back(new ThreadWrapper(name, isCore, task,
				this));
			return true;
		}

		bool ThreadPool::rejectExecution(const TaskType& task)
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