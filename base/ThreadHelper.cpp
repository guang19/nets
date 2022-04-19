//
// Created by YangGuang on 2022/4/9.
//


#include "base/ThreadHelper.h"

#include <sys/syscall.h>
#include <unistd.h>

namespace nets
{
	namespace base
	{

		__thread pid_t cacheTid_ = 0;

		pid_t getTid()
		{
			return static_cast<pid_t>(::syscall(SYS_gettid));
		}

		pid_t currentTid()
		{
			if (cacheTid_ <= 0)
			{
				cacheTid_ = getTid();
			}
			return cacheTid_;
		}
	}
}