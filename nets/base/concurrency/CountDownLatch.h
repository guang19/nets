//
// Created by YangGuang on 2022/5/4.
//

#ifndef NETS_COUNTDOWNLATCH_H
#define NETS_COUNTDOWNLATCH_H

#include <cstdint>
#include "nets/base/concurrency/ConditionVariable.h"
#include "nets/base/concurrency/Mutex.h"
#include "nets/base/Noncopyable.h"

namespace nets
{
	namespace base
	{
		class CountDownLatch : Noncopyable
		{
			public:
				explicit CountDownLatch(uint32_t count);

			public:
				void wait();
				void countDown();
				uint32_t getCount();

			private:
				Mutex mutex_ {};
				ConditionVariable cv_{};
				uint32_t count_ { 0 };
		};
	} // namespace base
} // namespace nets

#endif //NETS_COUNTDOWNLATCH_H