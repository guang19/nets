//
// Created by YangGuang on 2022/4/9.
//

#ifndef NETS_THREADHELPER_H
#define NETS_THREADHELPER_H

#include <stdint.h>
#include <sys/types.h>

namespace nets
{
	namespace base
	{
		__thread pid_t cacheTid_ = 0;
		__thread char tidString_[32] = { 0 };
		__thread uint32_t tidStringLength_ = 0;

		pid_t getTid();
		pid_t currentTid();
	} // namespace base
} // namespace nets

#endif //NETS_THREADHELPER_H
