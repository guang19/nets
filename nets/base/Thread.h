//
// Created by YangGuang on 2022/4/30.
//

#ifndef NETS_THREAD_H
#define NETS_THREAD_H

#include <atomic>
#include <functional>
#include <pthread.h>
#include "nets/base/ThreadHelper.h"
#include "nets/base/Noncopyable.h"
#include "nets/base/concurrency/CountDownLatch.h"

namespace nets
{
	namespace base
	{
		class Thread : Noncopyable
		{
			public:
				using ThreadFunc = ::std::function<void ()>;

			public:
				Thread(ThreadFunc threadFunc, const ::std::string& threadName = "");
				~Thread();

			public:
				void start();
				void join();
				void detach();

				inline ::pthread_t getThreadId() const
				{
					return threadId_;
				}

				inline ::pthread_t getTid() const
				{
					return tid_;
				}

				inline const ::std::string& getThreadName() const
				{
					return threadName_;
				}

				inline bool joinable() const
				{
					return joinable_;
				}

			private:
				void setDefaultThreadName();

			private:
				bool runnable_ { false };
				bool joinable_ { false };
				::pthread_t threadId_ { 0 };
				::pid_t tid_ { 0 };
				::std::string threadName_ {};
				ThreadFunc threadFunc_ {};
				CountDownLatch latch_ { 0 };

				static ::std::atomic<uint32_t> NumOfCreatedThreads;
		};
	} // namespace base
} // namespace nets


#endif //NETS_THREAD_H
