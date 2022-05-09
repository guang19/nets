//
// Created by YangGuang on 2022/4/30.
//

#include "Thread.h"

#include <sys/prctl.h>

namespace nets
{
	namespace base
	{
		Thread::Thread() : Thread(nullptr)
		{
		}

		Thread::Thread(ThreadFunc  threadFunc, const std::string& threadName) : runnable_(false), joinable_(false),
			threadId_(0), tid_(0), threadName_(threadName), threadFunc_(std::move(threadFunc)), latch_(1)
		{
		}

		Thread::~Thread()
		{
			if (joinable_ && runnable_)
			{
				pthread_detach(threadId_);
			}
		}

		namespace
		{
			class ThreadRouter
			{
				public:
					ThreadRouter(const ::std::string& threadName, pid_t* tid, Thread::ThreadFunc threadFunc, CountDownLatch& latch) :
						threadName_(threadName), tid_(tid), threadFunc_(::std::move(threadFunc)), latch_(latch)
					{
					}

					~ThreadRouter() = default;

					void route()
					{
						*tid_ = currentTid();
						if (!threadName_.empty())
						{
							setCurrentThreadName(threadName_.c_str());
						}
						::prctl(PR_SET_NAME, currentThreadName());
						latch_.countDown();
						try
						{
							threadFunc_();
						}
						catch (const ::std::exception& exception)
						{
							::fprintf(::stderr, "Error:exception caught in thread %s,reason %s\n",
								threadName_.c_str(), exception.what());
							::exit(1);
						}
						catch (...)
						{
							::fprintf(::stderr, "Error:unknown exception caught in thread %s\n", threadName_.c_str());
							::exit(1);
						}
					}

				private:
					::std::string threadName_ {};
					::pid_t* tid_ { nullptr };
					Thread::ThreadFunc threadFunc_;
					CountDownLatch& latch_;
			};

			void* startThread(void* arg)
			{
				ThreadRouter* router = static_cast<ThreadRouter*>(arg);
				router->route();
				delete router;
				return nullptr;
			}
		}

		void Thread::start()
		{
			setDefaultThreadName();
			ThreadRouter* router = new ThreadRouter(threadName_, &tid_, threadFunc_, latch_);
			runnable_ = true;
			joinable_ = true;
			if (pthread_create(&threadId_, nullptr, startThread, router) != 0)
			{
				delete router;
				router = nullptr;
				runnable_ = false;
				joinable_ = false;
			}
			else
			{
				latch_.wait();
			}
		}

		void Thread::join()
		{
			if (joinable_ && runnable_)
			{
				joinable_ = false;
				pthread_join(threadId_, nullptr);
			}
		}

		void Thread::detach()
		{
			if (joinable_ && runnable_)
			{
				joinable_ = false;
				pthread_detach(threadId_);
			}
		}

		::std::atomic<uint32_t> Thread::NumOfCreatedThreads { 0 };

		void Thread::setDefaultThreadName()
		{
			NumOfCreatedThreads += 1;
			if (threadName_.empty())
			{
				char name[24] = { 0 };
				::snprintf(name, 24, "NesThread-%u", NumOfCreatedThreads.load());
				threadName_ = name;
			}
		}
	} // namespace base
} // namespace nets
