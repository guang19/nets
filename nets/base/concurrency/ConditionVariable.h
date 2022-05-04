//
// Created by YangGuang on 2022/5/2.
//

#ifndef NETS_CONDITIONVARIABLE_H
#define NETS_CONDITIONVARIABLE_H

#include <chrono>
#include "nets/base/concurrency/Mutex.h"

namespace nets
{
	namespace base
	{
		class ConditionVariable : Noncopyable
		{
			public:
				ConditionVariable();
				~ConditionVariable();

			public:
				void notifyOne();
				void notifyAll();
				void wait(Mutex& mutex);
				bool wait(Mutex& mutex, ::std::time_t seconds);

			private:
				pthread_cond_t condition_;
				Mutex mutex_;
		};
	} // namespace base
} // namespace nets

#endif //NETS_CONDITIONVARIABLE_H