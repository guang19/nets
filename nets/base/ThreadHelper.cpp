//
// Created by YangGuang on 2022/4/9.
//


#include "nets/base/ThreadHelper.h"

#include <sys/syscall.h>
#include <unistd.h>
#include "nets/base/Timestamp.h"

namespace nets
{
	namespace base
	{

		__thread ::pid_t cacheTid_ = 0;

		::pid_t getTid()
		{
			return static_cast<pid_t>(::syscall(SYS_gettid));
		}

		::pid_t currentTid()
		{
			if (cacheTid_ <= 0)
			{
				cacheTid_ = getTid();
			}
			return cacheTid_;
		}

		bool isMainThread()
		{
			return ::getpid() == currentTid();
		}

		__thread const char* threadName_ = "unnamed";

		namespace
		{
			constexpr const char* const MainThreadName = "Main";
		}

		void afterFork()
		{
			cacheTid_ = 0;
			threadName_ = MainThreadName;
		}

		struct ThreadInitializer
		{
			ThreadInitializer()
			{
				currentTid();
				threadName_ = MainThreadName;
				pthread_atfork(nullptr, nullptr, &afterFork);
			}
		};

		ThreadInitializer threadInitializer {};

		void setCurrentThreadName(const char* threadName)
		{
			threadName_ = threadName;
		}

		const char* currentThreadName()
		{
			return threadName_;
		}

		void sleepS(TimeType seconds)
		{
			::sleep(seconds);
		}

		void sleepMillis(TimeType mseconds)
		{
			::usleep(mseconds * 1000);
		}

		void sleepMicros(TimeType useconds)
		{
			::usleep(useconds);
		}

		void sleepNanos(TimeType nseconds)
		{
			struct timespec timSpec {};
			timSpec.tv_sec = (nseconds / NanosecondsPerSecond);
			timSpec.tv_nsec = nseconds % NanosecondsPerSecond;
			::nanosleep(&timSpec, nullptr);
		}
	}
}