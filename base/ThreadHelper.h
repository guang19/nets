//
// Created by YangGuang on 2022/4/9.
//

#ifndef NETS_THREADHELPER_H
#define NETS_THREADHELPER_H

#include <thread>
#include <sys/types.h>

namespace nets
{
	namespace base
	{
		pid_t getTid();
		pid_t currentTid();
	} // namespace base
} // namespace nets

#endif //NETS_THREADHELPER_H
