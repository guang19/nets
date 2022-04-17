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
		extern __thread pid_t cacheTid_;

		pid_t getTid();
		pid_t currentTid();
	} // namespace base
} // namespace nets

#endif //NETS_THREADHELPER_H
