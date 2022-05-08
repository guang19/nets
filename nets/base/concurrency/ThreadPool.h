//
// Created by guang19 on 2021/12/30.
//

#ifndef NETS_BASE_THREAD_POOL_H
#define NETS_BASE_THREAD_POOL_H

#include <atomic>
#include <functional>
#include <vector>
#include <memory>
#include "nets/base/concurrency/BoundedBlockingQueue.h"
#include "nets/base/concurrency/Mutex.h"
#include "nets/base/log/Logging.h"
#include "nets/base/Noncopyable.h"
#include "nets/base/Thread.h"

namespace nets
{
	namespace base
	{
		class ThreadPool : Noncopyable
		{
			public:
				using SizeType = ::std::size_t;
				using TaskType = ::std::function<void()>;
				using MutexType = Mutex;
				using LockGuardType = LockGuard<MutexType>;
				using AtomicBoolType = ::std::atomic<bool>;
				using ThreadPoolRawPtr = ThreadPool*;
				using ConditionVariableType = ConditionVariable;
				using BoundedBlockingQueuePtr = ::std::unique_ptr<BoundedBlockingQueue<TaskType>>;

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
						explicit ThreadWrapper(const ::std::string& name, bool isCoreThread,
											   ThreadPoolRawPtr threadPoolRawPtr);

						explicit ThreadWrapper(const ::std::string& name, bool isCoreThread, TaskType task,
											   ThreadPoolRawPtr threadPoolRawPtr);

						~ThreadWrapper() = default;

						void startThread();

						bool isCoreThread_{false};
						TaskType task_{nullptr};
						Thread thread_{};
						ThreadPoolRawPtr threadPoolRawPtr_{nullptr};
				};

				using ThreadWrapperRawPtr = ThreadWrapper*;
				using ThreadWrapperPtr = ::std::unique_ptr<ThreadWrapper>;

			public:
				explicit ThreadPool(SizeType corePoolSize, SizeType maximumPoolSize);

				explicit ThreadPool(SizeType corePoolSize, SizeType maximumPoolSize, ::std::time_t keepAliveTime);

				explicit ThreadPool(SizeType corePoolSize, SizeType maximumPoolSize, ::std::time_t keepAliveTime,
									SizeType maxQueueSize);

				explicit ThreadPool(const ::std::string& name, SizeType corePoolSize, SizeType maximumPoolSize,
									enum RejectionPolicy rejectionPolicy);

				explicit ThreadPool(const ::std::string& name, SizeType corePoolSize, SizeType maximumPoolSize,
									::std::time_t keepAliveTime, enum RejectionPolicy rejectionPolicy);

				explicit ThreadPool(const ::std::string& name, SizeType corePoolSize, SizeType maximumPoolSize,
									::std::time_t keepAliveTime, SizeType maxQueueSize,
									enum RejectionPolicy rejectionPolicy);

				~ThreadPool();

				void init();

				void shutdown();

				inline bool isRunning() const
				{
					return running_;
				}

				inline const ::std::string& getThreadPoolName() const
				{
					return name_;
				}

				inline SizeType getPoolSize()
				{
					LockGuardType lock(mutex_);
					return threadPool_.size();
				}

				template<typename Fn, typename... Args>
				bool execute(Fn func, Args... args);

			private:
				void runThread(ThreadWrapperRawPtr threadWrapperRawPtr);

				void releaseThread(ThreadWrapperRawPtr threadWrapperRawPtr);

				bool addThreadTask(const TaskType& task, bool isCore, SizeType threadSize);

				bool rejectExecution(const TaskType& task);

			private:
				::std::string name_{};
				AtomicBoolType running_{false};
				// the numbers of core threads, once created,
				// will be destroyed as the life cycle of the thread pool ends
				SizeType corePoolSize_{0};
				// 线程池最大容纳的线程数
				// the maximum numbers of threads that the thread pool can hold
				SizeType maximumPoolSize_{0};
				// time that idle threads can survive, unit: ms
				::std::time_t keepAliveTime_{0};
				// task queue
				BoundedBlockingQueuePtr taskQueue_{nullptr};
				::std::vector<ThreadWrapperPtr> threadPool_{};
				MutexType mutex_{};
				ConditionVariableType poolCV_{};
				// rejection policy
				RejectionPolicy rejectionPolicy_;
		};

		template<typename Fn, typename... Args>
		bool ThreadPool::execute(Fn func, Args... args)
		{
			if (!running_)
			{
				LOGS_ERROR << "thread pool has not been initialized";
				return false;
			}
			LockGuardType lock(mutex_);
			::std::function<decltype(::std::declval<Fn>()(::std::declval<Args>()...))()> originFunc =
				::std::bind(::std::forward<Fn>(func), ::std::forward<Args>(args)...);
			TaskType task = [originFunc]()
			{
				originFunc();
			};
			SizeType threadSize = threadPool_.size();
			// if still able to create core thread
			if (threadSize < corePoolSize_)
			{
				return addThreadTask(task, true, threadSize);
			}
			else
			{
				//if task queue is not full, try push task to task queue
				if (taskQueue_->tryPush(task))
				{
					return true;
				}
					// the thread pool is not full, create non-core thread to perform task
				else if (threadSize < maximumPoolSize_)
				{
					return addThreadTask(task, false, threadSize);
				}
					// handle task with rejection policy
				else
				{
					return rejectExecution(task);
				}
			}
		}
	} // namespace base
} // namespace nets

#endif // NETS_BASE_THREAD_POOL_H