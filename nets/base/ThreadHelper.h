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

		using TimeTp = ::time_t;

		void sleepS(TimeTp seconds);
		void sleepMillis(TimeTp mseconds);
		void sleepMicros(TimeTp useconds);
		void sleepNanos(TimeTp useconds);

	} // namespace base
} // namespace nets

#endif //NETS_THREADHELPER_H
