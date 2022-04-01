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
#include "base/Noncopyable.h"
#include "base/threadpool/BoundedBlockingQueue.hpp"

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
				using ThreadPoolRawPtr = ThreadPool *;
				using BoundedBlockingQueuePtr = ::std::unique_ptr<BoundedBlockingQueue<task_type>>;

			public:
				/**************************************************************************
				 *  当线程池已满，且任务队列已满时，新的任务无法被立刻执行，可以指定以下拒绝策略进行补偿
				 **************************************************************************/
				enum RejectionPolicy
				{
					// 丢弃新的任务，不做任何操作
					DiscardPolicy = 0,
					// 遗弃任务队列末尾的任务，再尝试把新任务添加到任务队列末尾
					DiscardOlderPolicy,
					// 使用调用者的线程直接执行任务
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
						size_type id_;
						bool isCoreThread_;
						task_type task_;
						::std::thread thread_;
						ThreadPoolRawPtr threadPoolRawPtr_;
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
				::std::string name_;
				atomic_bool_type running_;
				// 核心线程数，这部分线程属于常驻线程，一旦被创造就会随着线程池的周期结束而销毁
				size_type corePoolSize_;
				// 线程池最大容纳的线程数
				size_type maximumPoolSize_;
				// 线程池中除了常驻线程外，其余{ 空闲线程 }能够存活的时间
				milliseconds_type keepAliveTime_;
				// 任务队列
				BoundedBlockingQueuePtr taskQueue_;
				::std::list<ThreadWrapperPtr> pool_;
				mutex_type mtx_;
				condition_variable_type poolCv_;
				//拒绝策略
				RejectionPolicy rejectionPolicy_;

				static const ::std::string DefaultThreadPoolName;
				static const milliseconds_type DefaultKeepAliveTime;
				static const size_type DefaultMaxQueueSize;
		};

		const ::std::string ThreadPool::DefaultThreadPoolName = "NetsThreadPool";
		const ThreadPool::milliseconds_type ThreadPool::DefaultKeepAliveTime = milliseconds_type(30000);
		const ThreadPool::size_type ThreadPool::DefaultMaxQueueSize = INT32_MAX;

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
			task_type task = std::bind(originFunc);
			size_type threadSize = pool_.size();
			// 如果核心线程池未满，就创建新核心线程执行任务
			if (threadSize < corePoolSize_)
			{
				return addThreadTask(task, true, threadSize);
			}
			else
			{
				// 尝试直接添加到任务队列
				if (taskQueue_->tryPush(task))
				{
					return true;
				}
				// 线程池还未满，创建非核心线程执行任务
				if (threadSize < maximumPoolSize_)
				{
					return addThreadTask(task, false, threadSize);
				}
				else
				{
					// 拒绝策略
					return rejectedExecution(task);
				}
			}
		}
	} // namespace base
} // namespace nets

#endif // NETS_BASE_THREAD_POOL_H
