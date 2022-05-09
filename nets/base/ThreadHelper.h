//
// Created by YangGuang on 2022/4/9.
//

#ifndef NETS_THREADHELPER_H
#define NETS_THREADHELPER_H

#include <ctime>
#include <string>
#include <sys/types.h>

namespace nets::base
{
	::pid_t currentTid();
	bool isMainThread();

	void setCurrentThreadName(const char* threadName);
	const char* currentThreadName();

	using TimeType = ::time_t;

	void sleepS(TimeType seconds);
	void sleepMillis(TimeType mseconds);
	void sleepMicros(TimeType useconds);
	void sleepNanos(TimeType nseconds);
} // namespace nets::base

#endif // NETS_THREADHELPER_H
