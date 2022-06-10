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
		class ThreadWrapper;
		using ThreadWrapperRawPtr = ThreadWrapper*;
		using ThreadWrapperPtr = ::std::unique_ptr<ThreadWrapper>;
		using ThreadPoolType = ::std::vector<ThreadWrapperPtr>;

	public:
		using NType = uint32_t;
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
		class ThreadWrapper : Noncopyable
		{
		public:
			explicit ThreadWrapper(const char* threadName, bool isCoreThread, TaskType task, ThreadPoolPtr threadPool);
			~ThreadWrapper() = default;

			void start(ThreadPoolPtr threadPoolPtr);

			::std::string threadName_ {};
			bool isCoreThread_ {false};
			TaskType task_ {nullptr};
			::std::thread thread_ {};
		};

	public:
		explicit ThreadPool(NType corePoolSize, NType maxPoolSize, NType maxQueueSize,
							TimeType keepAliveTime = DefaultIdleKeepAliveTime,
							const ::std::string& name = DefaultThreadPoolName);
		~ThreadPool();

		inline bool isRunning() const
		{
			return isRunning(ctl_);
		}

		inline NType numOfActiveThreads() const
		{
			return numOfActiveThreads(ctl_);
		}

		inline const ::std::string& name() const
		{
			return name_;
		}

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Please note that if you need to use the returned future to get the result of the task execution, please use
		// try
		// {
		//     future.get();
		// }
		// catch(::std::future_error& e)
		// {
		//		// The thread pool was closed before the task was executed or the task was discarded
		// }
		// catch(::std::exception& e)
		// {
		// 		// The task throws an exception when it executes
		// }
		// catch(...)
		// {
		//  	// The task throws an unknown exception when it executes
		// }
		// to catch possible exceptions. The exceptions are as follows:
		// 1. An exception may occur when the submitted task is executed, and the promise sets the abnormal result;
		// 2. After the task is submitted, the thread pool shuts down before the task is executed;
		// 3. The thread pool is full and The task queue cannot receive new tasks, then the task will be discarded;

		template <typename Fn, typename... Args>
		void execute(Fn&& func, Args&&... args);

		template <typename Fn, typename... Args,
				  typename HasRet = typename ::std::enable_if<
					  !::std::is_void<typename ::std::invoke_result<Fn&&, Args&&...>::type>::value>::type>
		::std::future<typename ::std::invoke_result<Fn&&, Args&&...>::type> submit(Fn&& func, Args&&... args);

		template <typename Fn, typename... Args,
				  typename HasRet = typename ::std::enable_if<
					  ::std::is_void<typename ::std::invoke_result<Fn&&, Args&&...>::type>::value>::type>
		::std::future<void> submit(Fn&& func, Args&&... args);
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	private:
		template <typename RetType>
		TaskType makeTask(const ::std::shared_ptr<::std::promise<RetType>>& promise, ::std::function<RetType()> promiseTask);

		TaskType makeTask(const ::std::shared_ptr<::std::promise<void>>& promise, ::std::function<void()> promiseTask);

	private:
		void tryShutdown();
		void runThread(ThreadWrapperRawPtr threadWrapper);
		void releaseThread(ThreadWrapperRawPtr threadWrapperRawPtr);
		bool addThreadTask(const TaskType& task, bool isCore);

		inline bool isRunning(NType ctl) const
		{
			return (ctl & ~CountMask) == Running;
		}

		inline bool isShutdown(NType ctl) const
		{
			return (ctl & ~CountMask) == Shutdown;
		}

		inline NType numOfActiveThreads(NType ctl) const
		{
			return ctl & CountMask;
		}

	private:
		static constexpr NType Int32Bits = 32;
		static constexpr NType CountBits = Int32Bits - 2;
		// maximum active thread size
		// 00,111111111111111111111111111111
		static constexpr NType CountMask = (1 << CountBits) - 1;
		// 01,000000000000000000000000000000
		static constexpr NType Running = 1 << CountBits;
		// 00,000000000000000000000000000000
		static constexpr NType Shutdown = 0 << CountBits;

		static constexpr TimeType DefaultIdleKeepAliveTime = 30000;
		static constexpr char const* DefaultThreadPoolName = "NetsThreadPool";

	private:
		// the numbers of core threads, once created, will be destroyed as the life cycle of the thread pool ends
		NType corePoolSize_ {0};
		// the maximum numbers of threads that the thread pool can hold
		NType maximumPoolSize_ {0};
		// time that idle threads can survive, unit: ms
		TimeType idleKeepAliveTime_ {0};
		// task queue
		BlockingQueuePtr taskQueue_ {nullptr};
		ThreadPoolType threadPool_ {};
		::std::string name_ {};
		// high 2bits represent thread pool status: 00 - shutdown; 01-running.
		// low 30bits represent thread pool active thread size.
		::std::atomic_uint32_t ctl_ {0};
		MutexType mutex_ {};
		ConditionVariableType poolCV_ {};
	};

	template <typename Fn, typename... Args>
	void ThreadPool::execute(Fn&& func, Args&&... args)
	{
		submit(::std::forward<Fn>(func), ::std::forward<Args>(args)...);
	}

	template <typename Fn, typename... Args, typename HasRet>
	::std::future<typename ::std::invoke_result<Fn&&, Args&&...>::type> ThreadPool::submit(Fn&& func, Args&&... args)
	{
		using RetType = typename ::std::invoke_result<Fn&&, Args&&...>::type;
		auto promise = ::std::make_shared<::std::promise<RetType>>();
		auto future = promise->get_future();
		::std::function<RetType()> promiseTask = ::std::bind(::std::forward<Fn>(func), ::std::forward<Args>(args)...);
		TaskType task = makeTask<RetType>(promise, promiseTask);
		assert(2 == promise.use_count());
		NType ctl = ctl_.load();
		assert(isRunning(ctl));
		if (isShutdown(ctl))
		{
			LOGS_ERROR << "thread pool [" << name_ << "] has not been initialized";
			promise->set_exception(::std::make_exception_ptr(::std::future_error(::std::future_errc::no_state)));
			return future;
		}
		// if num of active threads less than num of corePoolSize
		if (numOfActiveThreads(ctl) < corePoolSize_)
		{
			if (addThreadTask(task, true))
			{
				return future;
			}
		}
		// add task to core thread failed, update ctl
		ctl = ctl_.load();
		// if task queue is not full, try push task to task queue
		if (isRunning(ctl) && taskQueue_->tryPush(task))
		{
			return future;
		}
		// task queue cannot receive new task, create non-core thread to execute task
		if (addThreadTask(task, false))
		{
			return future;
		}
		promise->set_exception(::std::make_exception_ptr(::std::future_error(::std::future_errc::no_state)));
		return future;
	}

	template <typename Fn, typename... Args, typename HasRet>
	::std::future<void> ThreadPool::submit(Fn&& func, Args&&... args)
	{
		auto promise = ::std::make_shared<::std::promise<void>>();
		auto future = promise->get_future();
		::std::function<void()> promiseTask = ::std::bind(::std::forward<Fn>(func), ::std::forward<Args>(args)...);
		TaskType task = makeTask(promise, promiseTask);
		assert(2 == promise.use_count());
		NType ctl = ctl_.load();
		assert(isRunning(ctl));
		if (isShutdown(ctl))
		{
			LOGS_ERROR << "thread pool [" << name_ << "] has not been initialized";
			promise->set_exception(::std::make_exception_ptr(::std::future_error(::std::future_errc::no_state)));
			return future;
		}
		// if num of active threads less than num of corePoolSize
		if (numOfActiveThreads(ctl) < corePoolSize_)
		{
			if (addThreadTask(task, true))
			{
				return future;
			}
		}
		// add task to core thread failed, update ctl
		ctl = ctl_.load();
		// if task queue is not full, try push task to task queue
		if (isRunning(ctl) && taskQueue_->tryPush(task))
		{
			return future;
		}
		// task queue cannot receive new task, create non-core thread to execute task
		if (addThreadTask(task, false))
		{
			return future;
		}
		promise->set_exception(::std::make_exception_ptr(::std::future_error(::std::future_errc::no_state)));
		return future;
	}

	template <typename RetType>
	ThreadPool::TaskType ThreadPool::makeTask(const ::std::shared_ptr<::std::promise<RetType>>& promise,
											  ::std::function<RetType()> promiseTask)
	{
		TaskType task = [this, promise, f = ::std::move(promiseTask)]()
		{
			assert(promise.use_count() > 0);
			try
			{
				RetType r = f();
				promise->set_value(r);
			}
			catch (const ::std::exception& exception)
			{
				promise->set_exception(::std::make_exception_ptr(exception));
				LOGS_ERROR << "exception caught during thread [" << currentThreadName() << "] execution in thread pool ["
						   << name_ << "], reason " << exception.what();
			}
			catch (...)
			{
				promise->set_exception(::std::current_exception());
				LOGS_ERROR << "unknown exception caught during thread [" << currentThreadName()
						   << "] execution in thread pool [" << name_ << ']';
			}
		};
		return task;
	}

	ThreadPool::TaskType ThreadPool::makeTask(const ::std::shared_ptr<::std::promise<void>>& promise,
											  ::std::function<void()> promiseTask)
	{
		TaskType task = [this, promise, f = ::std::move(promiseTask)]()
		{
			try
			{
				f();
				promise->set_value();
			}
			catch (const ::std::exception& exception)
			{
				promise->set_exception(::std::make_exception_ptr(exception));
				LOGS_ERROR << "exception caught during thread [" << currentThreadName() << "] execution in thread pool ["
						   << name_ << "], reason " << exception.what();
			}
			catch (...)
			{
				promise->set_exception(::std::current_exception());
				LOGS_ERROR << "unknown exception caught during thread [" << currentThreadName()
						   << "] execution in thread pool [" << name_ << ']';
			}
		};
		return task;
	}
} // namespace nets::base

#endif // NETS_BASE_THREAD_POOL_H