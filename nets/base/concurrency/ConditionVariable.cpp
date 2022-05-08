//
// Created by YangGuang on 2022/5/2.
//

#include "nets/base/concurrency/ConditionVariable.h"

#include "nets/base/Timestamp.h"

namespace nets
{
	namespace base
	{
		ConditionVariable::ConditionVariable()
		{
			pthread_cond_init(&condition_, nullptr);
		}

		ConditionVariable::~ConditionVariable()
		{
			pthread_cond_destroy(&condition_);
		}

		void ConditionVariable::notifyOne()
		{
			pthread_cond_signal(&condition_);
		}

		void ConditionVariable::notifyAll()
		{
			pthread_cond_broadcast(&condition_);
		}

		void ConditionVariable::wait(Mutex &mutex)
		{
			// during waiting time, other threads may acquire ownership of the parameter(@mutex)
			// so modify the ownership of the mutex in advance
			Mutex::OwnerGuard ownerGuard(mutex);
			pthread_cond_wait(&condition_, mutex.getMutexPtr());
		}

		bool ConditionVariable::waitTimeout(Mutex &mutex, TimeType milliseconds)
		{
			struct timespec tmSpec{};
			::clock_gettime(CLOCK_REALTIME, &tmSpec);
			TimeType nanoseconds = static_cast<TimeType>(milliseconds * MicrosecondsPerSecond);
			tmSpec.tv_sec += static_cast<TimeType>((tmSpec.tv_nsec + nanoseconds) / NanosecondsPerSecond);
			tmSpec.tv_nsec += static_cast<int64_t>((tmSpec.tv_nsec + nanoseconds) % NanosecondsPerSecond);
			// tv_nsec can not exceed <NanosecondsPerSecond>,
			// otherwise pthread_cond_timedwait will have an unknown result
			while (tmSpec.tv_nsec >= NanosecondsPerSecond)
			{
				tmSpec.tv_sec += 1;
				tmSpec.tv_nsec -= NanosecondsPerSecond;
			}
			// during waiting time, other threads may acquire ownership of the parameter(@mutex)
			// so modify the ownership of the mutex in advance
			Mutex::OwnerGuard ownerGuard(mutex);
			return (0 == pthread_cond_timedwait(&condition_, mutex.getMutexPtr(), &tmSpec));
		}
	} // namespace base
} // namespace nets
