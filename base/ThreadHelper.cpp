//
// Created by YangGuang on 2022/4/9.
//


#include "base/ThreadHelper.h"
#include <stdio.h>
#include <sys/syscall.h>
#include <unistd.h>

namespace nets
{
	namespace base
	{
		pid_t getTid()
		{
			return static_cast<pid_t>(::syscall(SYS_gettid));
		}

		pid_t currentTid()
		{
			if (cacheTid_ == 0)
			{
				cacheTid_ = getTid();
				tidStringLength_ = snprintf(tidString_, sizeof(tidString_), "%d", cacheTid_);
			}
			return cacheTid_;
		}
	}
}