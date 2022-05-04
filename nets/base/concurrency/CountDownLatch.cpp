//
// Created by YangGuang on 2022/5/4.
//

#include "nets/base/concurrency/CountDownLatch.h"

namespace nets
{
	namespace base
	{
		CountDownLatch::CountDownLatch(uint32_t count) : count_(count)
		{
		}

		void CountDownLatch::wait()
		{
			LockGuard<Mutex> lock(mutex_);
			while (count_ > 0)
			{
				cv_.wait(mutex_);
			}
		}

		void CountDownLatch::countDown()
		{
			LockGuard<Mutex> lock(mutex_);
			count_ -= 1;
			if (count_ == 0)
			{
				cv_.notifyAll();
			}
		}

		uint32_t CountDownLatch::getCount()
		{
			LockGuard<Mutex> lock(mutex_);
			return count_;
		}
	}
}
