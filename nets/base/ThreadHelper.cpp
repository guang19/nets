//
// Created by YangGuang on 2022/4/9.
//

#include "nets/base/ThreadHelper.h"

#include <cstring>
#include <sys/syscall.h>
#include <unistd.h>

#include "nets/base/CommonMacro.h"

namespace nets::base
{
	__thread ::pid_t cacheTid_ = 0;
	__thread char threadName_[ThreadNameMaxLength] = "unnamed";

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

	namespace
	{
		constexpr const char* const MainThreadName = "Main";
	}

	void afterFork()
	{
		cacheTid_ = 0;
		MEMZERO(threadName_, ThreadNameMaxLength);
		::memcpy(threadName_, MainThreadName, strlen(MainThreadName));
	}

	struct ThreadInitializer
	{
		ThreadInitializer()
		{
			currentTid();
			MEMZERO(threadName_, ThreadNameMaxLength);
			::memcpy(threadName_, MainThreadName, strlen(MainThreadName));
			::pthread_atfork(nullptr, nullptr, &afterFork);
		}
	};

	ThreadInitializer threadInitializer {};

	bool setPosixThreadName(::pthread_t threadId, const char* threadName)
	{
		return (::pthread_setname_np(threadId, threadName) == 0);
	}

	void setThreadName(::pthread_t threadId, const char* threadName)
	{
		if (setPosixThreadName(threadId, threadName))
		{
			MEMZERO(threadName_, ThreadNameMaxLength);
			::memcpy(threadName_, threadName, strlen(threadName));
		}
	}

	void getThreadName(::pthread_t threadId, char* threadName, int32_t len)
	{
		pthread_getname_np(threadId, threadName, len);
	}

	void setCurrentThreadName(const char* threadName)
	{
		setThreadName(pthread_self(), threadName);
	}

	const char* currentThreadName()
	{
		return threadName_;
	}
} // namespace nets::base
