//
// Created by YangGuang on 2022/4/9.
//

#ifndef NETS_THREADHELPER_H
#define NETS_THREADHELPER_H

#include <cstdint>
#include <pthread.h>

namespace nets::base
{
	namespace
	{
		constexpr int32_t ThreadNameMaxLength = 16;
	}

	::pid_t currentTid();
	bool isMainThread();

	void setThreadName(::pthread_t threadId, const char* threadName);
	void getThreadName(::pthread_t threadId, char* threadName, int32_t len);

	void setCurrentThreadName(const char* threadName);
	const char* currentThreadName();
} // namespace nets::base

#endif // NETS_THREADHELPER_H
