//
// Created by guang19 on 2021/12/30.
//

#ifndef NETS_BASE_THREAD_POOL_H
#define NETS_BASE_THREAD_POOL_H

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>
#include <list>
#include "nets/base/Noncopyable.h"
#include "nets/base/concurrency/BoundedBlockingQueue.h"

namespace nets
{
	namespace base
	{
		class ThreadPool : Noncopyable
		{
			public:
				using size_type = ::std::size_t;
				using atomic_bool_type = ::std::atomic<bool>;
				using task_type = ::std::function<void()>;
				using milliseconds_type = ::std::chrono::milliseconds;
				using mutex_type = ::std::mutex;
				using condition_variable_type = ::std::condition_variable;
				using lock_guard_type = ::std::lock_guard<::std::mutex>;
				using unique_lock_type = ::std::unique_lock<::std::mutex>;
				using ThreadPoolRawPtr = ThreadPool*;
				using BoundedBlockingQueuePtr = ::std::unique_ptr<BoundedBlockingQueue<task_type>>;

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
					// and try to add a new task to the end
					DiscardOlderPolicy,
					// the caller's thread runs the task directly
					CallerRunsPolicy,
				};

			private:
				class ThreadWrapper : Noncopyable
				{
					public:
						ThreadWrapper(size_type id, bool isCoreThread, ThreadPoolRawPtr threadPoolRawPtr);

						ThreadWrapper(size_type id, bool isCoreThread, const task_type &task,
									  ThreadPoolRawPtr threadPoolRawPtr);

						~ThreadWrapper() = default;

					private:
						void startThread();

					public:
						size_type id_ { 0 };
						bool isCoreThread_ { false };
						task_type task_ { nullptr };
						::std::thread thread_ {};
						ThreadPoolRawPtr threadPoolRawPtr_ { nullptr };
				};

				using ThreadWrapperRawPtr = ThreadWrapper*;
				using ThreadWrapperPtr = ::std::unique_ptr<ThreadWrapper>;

			private:
				void runThread(ThreadWrapperRawPtr threadWrapperRawPtr);

				void releaseThread(ThreadWrapperRawPtr threadWrapperRawPtr);

				bool addThreadTask(const task_type &task, bool isCore, size_type currentThreadSize);

				bool rejectedExecution(const task_type& task);

			public:
				ThreadPool(size_type corePoolSize, size_type maximumPoolSize);

				ThreadPool(size_type corePoolSize, size_type maximumPoolSize, const milliseconds_type &keepAliveTime);

				ThreadPool(size_type corePoolSize, size_type maximumPoolSize, const milliseconds_type &keepAliveTime,
						   size_type maxQueueSize);

				ThreadPool(const ::std::string &name, size_type corePoolSize, size_type maximumPoolSize);

				ThreadPool(const ::std::string &name, size_type corePoolSize, size_type maximumPoolSize,
						   const milliseconds_type &keepAliveTime);

				ThreadPool(const ::std::string &name, size_type corePoolSize, size_type maximumPoolSize,
						   const milliseconds_type &keepAliveTime, size_type maxQueueSize);

				~ThreadPool();

				void init();
				void shutdown();

				template<typename Fn, typename... Args>
				bool execute(Fn fun, Args... args);

				bool isRunning() const
				{
					return running_;
				}

				const ::std::string &name() const
				{
					return name_;
				}

				size_type poolSize()
				{
					lock_guard_type lock(mtx_);
					return pool_.size();
				}

				void setRejectionPolicy(RejectionPolicy rejectionPolicy)
				{
					rejectionPolicy_ = rejectionPolicy;
				}

			private:
				::std::string name_ {};
				atomic_bool_type running_ { false };
				// the numbers of core threads, once created,
				// will be destroyed as the life cycle of the thread pool ends
				size_type corePoolSize_ { 0 };
				// 线程池最大容纳的线程数
				// the maximum numbers of threads that the thread pool can hold
				size_type maximumPoolSize_ { 0 };
				// time that idle threads can survive
				milliseconds_type keepAliveTime_ { 0 };
				// task queue
				BoundedBlockingQueuePtr taskQueue_ { nullptr };
				::std::list<ThreadWrapperPtr> pool_ {};
				mutex_type mtx_ {};
				condition_variable_type poolCv_ {};
				// rejection policy
				RejectionPolicy rejectionPolicy_;
		};



		template<typename Fn, typename... Args>
		bool ThreadPool::execute(Fn fun, Args... args)
		{

			if (!running_)
			{
				return false;
			}
			lock_guard_type lock(mtx_);
			::std::function<typename ::std::result_of<Fn(Args...)>::type ()> originFunc =
					::std::bind(::std::forward<Fn>(fun), ::std::forward<Args>(args)...);
			task_type task = ::std::bind(originFunc);
			size_type threadSize = pool_.size();
			// if still able to create core thread
			if (threadSize < corePoolSize_)
			{
				return addThreadTask(task, true, threadSize);
			}
			else
			{
				// try push task to task queue
				if (taskQueue_->tryPush(task))
				{
					return true;
				}
				// the thread pool is not full, create non-core thread to perform task
				if (threadSize < maximumPoolSize_)
				{
					return addThreadTask(task, false, threadSize);
				}
				else
				{
					// handle task with rejection policy
					return rejectedExecution(task);
				}
			}
		}
	} // namespace base
} // namespace nets

#endif // NETS_BASE_THREAD_POOL_H
