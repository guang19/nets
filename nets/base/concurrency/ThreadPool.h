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
	private:
		struct ThreadWrapper;
		using ThreadWrapperRawPtr = ThreadWrapper*;
		using ThreadWrapperPtr = ::std::unique_ptr<ThreadWrapper>;
		using ThreadPoolType = ::std::vector<ThreadWrapperPtr>;

	public:
		using TimeType = ::time_t;
		using TaskType = ::std::function<void()>;
		using MutexType = ::std::mutex;
		using LockGuardType = ::std::lock_guard<MutexType>;
		using UniqueLockType = ::std::unique_lock<MutexType>;
		using ConditionVariableType = ::std::condition_variable;
		using BlockingQueueType = BoundedBlockingQueue<TaskType>;
		using BlockingQueuePtr = ::std::unique_ptr<BlockingQueueType>;
		using ThreadPoolPtr = ThreadPool*;

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

	public:
		explicit ThreadPool(uint32_t corePoolSize = DefaultCorePoolSize, uint32_t maxPoolSize = DefaultMaxPoolSize,
							TimeType keepAliveTime = DefaultIdleKeepAliveTime, uint32_t maxQueueSize = DefaultMaxQueueSize,
							const ::std::string& name = DefaultThreadPoolName);
		~ThreadPool();

		void shutdown();

		inline bool isRunning() const
		{
			return isRunning(ctl_);
		}

		inline bool isShutdown() const
		{
			return isShutdown(ctl_);
		}

		inline uint32_t numOfActiveThreads()
		{
			return numOfActiveThreads(ctl_);
		}

		inline const ::std::string& name() const
		{
			return name_;
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
		bool addThreadTask(const TaskType& task, bool isCore);

		inline bool isRunning(uint32_t ctl) const
		{
			return (ctl & ~Capacity) == Running;
		}

		inline bool isShutdown(uint32_t ctl) const
		{
			return (ctl & ~Capacity) == Shutdown;
		}

		inline uint32_t numOfActiveThreads(uint32_t ctl) const
		{
			return ctl & Capacity;
		}

	private:
		static constexpr uint32_t Int32Bits = 32;
		static constexpr uint32_t CountBits = Int32Bits - 2;
		// maximum active thread size
		// 00,111111111111111111111111111111
		static constexpr uint32_t Capacity = (1 << CountBits) - 1;
		// 01,000000000000000000000000000000
		// 00,000000000000000000000000000000
		static constexpr uint32_t Running = 1 << CountBits;
		static constexpr uint32_t Shutdown = 0 << CountBits;

		static const uint32_t DefaultCorePoolSize;
		static const uint32_t DefaultMaxPoolSize;
		static constexpr TimeType DefaultIdleKeepAliveTime = 30000;
		static constexpr uint32_t DefaultMaxQueueSize = 24;
		static constexpr char const* DefaultThreadPoolName = "ThreadPool";

	private:
		::std::string name_ {};
		// high 2bits represent threadpool status: 00 - shutdown; 01-running.
		// low 30bits represent threadpool active thread size.
		::std::atomic_uint32_t ctl_ {false};
		// the numbers of core threads, once created, will be destroyed as the life cycle of the thread pool ends
		uint32_t corePoolSize_ {0};
		// the maximum numbers of threads that the threadpool can hold
		uint32_t maximumPoolSize_ {0};
		// time that idle threads can survive, unit: ms
		TimeType idleKeepAliveTime_ {0};
		// task queue
		BlockingQueuePtr taskQueue_ {nullptr};
		ThreadPoolType threadPool_ {};
		MutexType mutex_ {};
		ConditionVariableType poolCV_ {};
	};

	template <typename Fn, typename... Args>
	void ThreadPool::execute(Fn&& func, Args&&... args)
	{
		submit(::std::forward<Fn>(func), ::std::forward<Args>(args)...);
	}

	template <typename Fn, typename... Args, typename HasRet>
	::std::future<typename ::std::result_of<Fn(Args...)>::type> ThreadPool::submit(Fn&& func, Args&&... args)
	{
		assert(isRunning());
		if (isShutdown())
		{
			LOGS_FATAL << "threadpool [" << name_ << "] has not been initialized";
		}
		using RetType = typename ::std::result_of<Fn(Args...)>::type;
		auto promise = ::std::make_shared<::std::promise<RetType>>();
		auto future = promise->get_future();
		// value capture, ref count plus 1
		::std::function<RetType()> promiseTask = ::std::bind(::std::forward<Fn>(func), ::std::forward<Args>(args)...);
		TaskType task = makeTask<RetType>(promise, promiseTask);
		assert(2 == promise.use_count());
		uint32_t ctl = ctl_.load();
		uint32_t n = numOfActiveThreads(ctl);
		// if num of active threads less than num of corePoolSize
		if (n < corePoolSize_)
		{
			// add task to core thread failed, update ctl
			if (!addThreadTask(task, true))
			{
				ctl = ctl_.load();
			}
		}
		// if task queue is not full, try push task to task queue
		if (isRunning(ctl))
		{
			if (!taskQueue_->tryPush(task))
			{
				// the threadpool is full, create non-core thread to perform task
				addThreadTask(task, false);
			}
		}
		return future;
	}

	template <typename Fn, typename... Args, typename HasRet>
	::std::future<void> ThreadPool::submit(Fn&& func, Args&&... args)
	{
		assert(isRunning());
		if (isShutdown())
		{
			LOGS_FATAL << "threadpool [" << name_ << "] has not been initialized";
		}
		// value capture, ref count plus 1
		auto promise = ::std::make_shared<::std::promise<void>>();
		auto future = promise->get_future();
		::std::function<void()> promiseTask = ::std::bind(::std::forward<Fn>(func), ::std::forward<Args>(args)...);
		TaskType task = makeTask(promise, promiseTask);
		uint32_t ctl = ctl_.load();
		uint32_t n = numOfActiveThreads(ctl);
		// if num of active threads less than num of corePoolSize
		if (n < corePoolSize_)
		{
			// add task to core thread failed, update ctl
			if (!addThreadTask(task, true))
			{
				ctl = ctl_.load();
			}
		}
		// if task queue is not full, try push task to task queue
		if (isRunning(ctl))
		{
			if (!taskQueue_->tryPush(task))
			{
				// the threadpool is full, create non-core thread to perform task
				addThreadTask(task, false);
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
				LOGS_ERROR << "exception caught during thread [" << currentThreadName() << "] execution in threadpool ["
						   << name_ << "], reason " << exception.what();
			}
			catch (...)
			{
				LOGS_ERROR << "unknown exception caught during thread [" << currentThreadName()
						   << "] execution in threadpool [" << name_ << ']';
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
				LOGS_ERROR << "exception caught during thread [" << currentThreadName() << "] execution in threadpool ["
						   << name_ << "], reason " << exception.what();
			}
			catch (...)
			{
				LOGS_ERROR << "unknown exception caught during thread [" << currentThreadName()
						   << "] execution in threadpool [" << name_ << ']';
			}
		};
		return task;
	}
} // namespace nets::base

#endif // NETS_BASE_THREAD_POOL_H