//
// Created by guang19 on 2021/12/30.
//

#ifndef NETS_BASE_THREAD_POOL_H
#define NETS_BASE_THREAD_POOL_H

#include <atomic>
#include <cassert>
#include <functional>
#include <future>
#include <memory>
#include <thread>
#include <vector>

#include "nets/base/concurrency/BoundedBlockingQueue.h"
#include "nets/base/log/Logging.h"
#include "nets/base/Noncopyable.h"
#include "nets/base/ThreadHelper.h"

namespace nets::base
{
	class ThreadPool : Noncopyable
	{
	public:
		using SizeType = ::size_t;
		using TimeType = ::time_t;
		using TaskType = ::std::function<void()>;
		using MutexType = ::std::recursive_mutex;
		using LockGuardType = ::std::lock_guard<MutexType>;
		using UniqueLockType = ::std::unique_lock<MutexType>;
		using ConditionVariableType = ::std::condition_variable_any;
		using BlockingQueueType = BoundedBlockingQueue<TaskType>;
		using BlockingQueuePtr = ::std::unique_ptr<BlockingQueueType>;
		using ThreadPoolPtr = ThreadPool*;

	public:
		/**************************************************************************
		 * When the thread pool is full and the task queue cannot continue accept
		 * new tasks, the following rejection policy can be selected to deal with
		 * new tasks
		 **************************************************************************/
		enum RejectionPolicy
		{
			// abandon new tasks and do nothing
			DiscardPolicy = 0,
			// abandon the task at the end of the task queue
			// and try to add a new task to the end of task queue
			DiscardOlderPolicy,
			// the caller's thread runs the task directly
			CallerRunsPolicy,
		};

	private:
		struct ThreadWrapper : Noncopyable
		{
		public:
			explicit ThreadWrapper(const char* threadName, bool isCoreThread, TaskType task, ThreadPoolPtr threadPoolPtr);
			~ThreadWrapper() = default;

			void start();

			::std::string threadName_ {};
			bool isCoreThread_ {false};
			TaskType task_ {nullptr};
			::std::thread thread_ {};
			ThreadPoolPtr threadPoolPtr_ {nullptr};
		};

		using ThreadWrapperRawPtr = ThreadWrapper*;
		using ThreadWrapperPtr = ::std::unique_ptr<ThreadWrapper>;

	public:
		ThreadPool();

		explicit ThreadPool(SizeType corePoolSize, SizeType maxPoolSize);

		explicit ThreadPool(SizeType corePoolSize, SizeType maxPoolSize, TimeType keepAliveTime);

		explicit ThreadPool(SizeType corePoolSize, SizeType maxPoolSize, TimeType keepAliveTime, SizeType maxQueueSize);

		explicit ThreadPool(const ::std::string& name, SizeType corePoolSize, SizeType maxPoolSize,
							enum RejectionPolicy rejectionPolicy);

		explicit ThreadPool(const ::std::string& name, SizeType corePoolSize, SizeType maxPoolSize, TimeType keepAliveTime,
							enum RejectionPolicy rejectionPolicy);

		explicit ThreadPool(const ::std::string& name, SizeType corePoolSize, SizeType maxPoolSize, TimeType keepAliveTime,
							SizeType maxQueueSize, enum RejectionPolicy rejectionPolicy);

		~ThreadPool();

		void init();
		void shutdown();

		inline bool isRunning() const
		{
			return running_;
		}

		inline const ::std::string& name() const
		{
			return name_;
		}

		inline SizeType poolSize()
		{
			LockGuardType lock(mutex_);
			return threadPool_.size();
		}

		template <typename Fn, typename... Args>
		void execute(Fn&& func, Args&&... args);

		template <typename Fn, typename... Args,
				  typename HasRet =
					  typename ::std::enable_if<!::std::is_void<typename ::std::result_of<Fn(Args...)>::type>::value>::type>
		::std::future<typename ::std::result_of<Fn(Args...)>::type> submit(Fn&& func, Args&&... args);

		template <typename Fn, typename... Args,
				  typename HasRet =
					  typename ::std::enable_if<::std::is_void<typename ::std::result_of<Fn(Args...)>::type>::value>::type>
		::std::future<void> submit(Fn&& func, Args&&... args);

	private:
		template <typename RetType>
		TaskType makeTask(::std::shared_ptr<::std::promise<RetType>> promise, ::std::function<RetType()> promiseTask);

		TaskType makeTask(::std::shared_ptr<::std::promise<void>> promise, ::std::function<void()> promiseTask);

	private:
		void runThread(ThreadWrapperRawPtr threadWrapperRawPtr);
		void releaseThread(ThreadWrapperRawPtr threadWrapperRawPtr);
		bool addThreadTask(const TaskType& task, bool isCore, SizeType threadSize);
		void reject(const TaskType& task);

	private:
		::std::string name_ {};
		::std::atomic_bool running_ {false};
		// the numbers of core threads, once created,
		// will be destroyed as the life cycle of the thread pool ends
		SizeType corePoolSize_ {0};
		// the maximum numbers of threads that the thread pool can hold
		SizeType maxPoolSize_ {0};
		// time that idle threads can survive, unit: ms
		TimeType idleKeepAliveTime_ {0};
		// task queue
		BlockingQueuePtr taskQueue_ {nullptr};
		::std::vector<ThreadWrapperPtr> threadPool_ {};
		MutexType mutex_ {};
		ConditionVariableType poolCV_ {};
		// rejection policy
		RejectionPolicy rejectionPolicy_;
	};

	template <typename Fn, typename... Args>
	void ThreadPool::execute(Fn&& func, Args&&... args)
	{
		submit(::std::forward<Fn>(func), ::std::forward<Args>(args)...);
	}

	template <typename Fn, typename... Args, typename HasRet>
	::std::future<typename ::std::result_of<Fn(Args...)>::type> ThreadPool::submit(Fn&& func, Args&&... args)
	{
		assert(running_);
		if (!running_)
		{
			LOGS_FATAL << "thread pool has not been initialized";
		}
		LockGuardType lock(mutex_);
		using RetType = typename ::std::result_of<Fn(Args...)>::type;
		auto promise = ::std::make_shared<::std::promise<RetType>>();
		auto future = promise->get_future();
		// value capture, ref count plus 1
		::std::function<RetType()> promiseTask = ::std::bind(::std::forward<Fn>(func), ::std::forward<Args>(args)...);
		TaskType task = makeTask<RetType>(promise, promiseTask);
		assert(2 == promise.use_count());
		SizeType threadSize = threadPool_.size();
		// if still able to create core thread
		if (threadSize < corePoolSize_)
		{
			addThreadTask(task, true, threadSize);
		}
		else
		{
			// if task queue is not full, try push task to task queue
			if (!taskQueue_->tryPush(task))
			{
				// the thread pool is full, create non-core thread to perform task
				if (threadSize < maxPoolSize_)
				{
					addThreadTask(task, false, threadSize);
				}
				else
				{
					// handle task with rejection policy
					reject(task);
				}
			}
		}
		return future;
	}

	template <typename Fn, typename... Args, typename HasRet>
	::std::future<void> ThreadPool::submit(Fn&& func, Args&&... args)
	{
		assert(running_);
		if (!running_)
		{
			LOGS_FATAL << "thread pool has not been initialized";
		}
		LockGuardType lock(mutex_);
		// value capture, ref count plus 1
		auto promise = ::std::make_shared<::std::promise<void>>();
		auto future = promise->get_future();
		::std::function<void()> promiseTask = ::std::bind(::std::forward<Fn>(func), ::std::forward<Args>(args)...);
		TaskType task = makeTask(promise, promiseTask);
		SizeType threadSize = threadPool_.size();
		// if still able to create core thread
		if (threadSize < corePoolSize_)
		{
			addThreadTask(task, true, threadSize);
		}
		else
		{
			// if task queue is not full, try push task to task queue
			if (!taskQueue_->tryPush(task))
			{
				// the thread pool is full, create non-core thread to perform task
				if (threadSize < maxPoolSize_)
				{
					addThreadTask(task, false, threadSize);
				}
				else
				{
					// handle task with rejection policy
					reject(task);
				}
			}
		}
		return future;
	}

	template <typename RetType>
	ThreadPool::TaskType ThreadPool::makeTask(::std::shared_ptr<::std::promise<RetType>> promise,
											  ::std::function<RetType()> promiseTask)
	{
		TaskType task = [this, promise, t = ::std::move(promiseTask)]() mutable
		{
			assert(promise.use_count() > 0);
			try
			{
				promise->set_value(t());
			}
			catch (const ::std::exception& exception)
			{
				LOGS_ERROR << "exception caught during thread [" << currentThreadName() << "] execution in thread pool ["
						   << name_ << "], reason " << exception.what();
			}
			catch (...)
			{
				LOGS_ERROR << "unknown exception caught during thread [" << currentThreadName()
						   << "] execution in thread pool [" << name_ << ']';
			}
		};
		return task;
	}

	ThreadPool::TaskType ThreadPool::makeTask(::std::shared_ptr<::std::promise<void>> promise,
											  ::std::function<void()> promiseTask)
	{
		TaskType task = [this, promise, t = ::std::move(promiseTask)]() mutable
		{
			try
			{
				t();
				promise->set_value();
			}
			catch (const ::std::exception& exception)
			{
				LOGS_ERROR << "exception caught during thread [" << currentThreadName() << "] execution in thread pool ["
						   << name_ << "], reason " << exception.what();
			}
			catch (...)
			{
				LOGS_ERROR << "unknown exception caught during thread [" << currentThreadName()
						   << "] execution in thread pool [" << name_ << ']';
			}
		};
		return task;
	}
} // namespace nets::base

#endif // NETS_BASE_THREAD_POOL_H