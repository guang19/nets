//
// Created by YangGuang on 2022/4/9.
//

#ifndef NETS_THREADHELPER_H
#define NETS_THREADHELPER_H

#include <ctime>
#include <string>
#include <sys/types.h>

namespace nets
{
	namespace base
	{
		::pid_t currentTid();
		bool isMainThread();

		void setCurrentThreadName(const char* threadName);
		const char* getCurrentThreadName();

		void sleepS(::std::time_t seconds);
		void sleepMillis(::std::time_t mseconds);
		void sleepMicros(::std::time_t useconds);
		void sleepNanos(::std::time_t useconds);

	} // namespace base
} // namespace nets

#endif //NETS_THREADHELPER_H
