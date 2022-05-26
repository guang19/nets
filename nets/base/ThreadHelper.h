//
// Created by YangGuang on 2022/4/9.
//

#ifndef NETS_THREADHELPER_H
#define NETS_THREADHELPER_H

#include <cstdint>
#include <pthread.h>
#include <limits>

namespace nets::base
{
	namespace
	{
		//constexpr int32_t ThreadNameMaxLength = 16;
		constexpr int32_t ThreadNameMaxLength = ::std::numeric_limits<uint64_t>::digits10 + 8;
	}

	::pid_t currentTid();
	bool isMainThread();

	void setCurrentThreadName(const char* threadName);
	const char* currentThreadName();
} // namespace nets::base

#endif // NETS_THREADHELPER_H
