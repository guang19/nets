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
		// it‘s too short that the max length that unix/linux posix thread name is 16
		//constexpr int32_t ThreadNameMaxLength = 16;
		constexpr uint32_t ThreadNameMaxLength = 64;
	}

	::pid_t currentTid();
	bool isMainThread();

	void setCurrentThreadName(const char* threadName);
	const char* currentThreadName();
} // namespace nets::base

#endif // NETS_THREADHELPER_H
