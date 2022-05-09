//
// Created by YangGuang on 2022/5/2.
//

#include <gtest/gtest.h>

#include <functional>
#include <vector>

#include "nets/base/concurrency/CountDownLatch.h"
#include "nets/base/concurrency/Mutex.h"
#include "nets/base/Thread.h"

using namespace nets::base;

TEST(ConcurrencyTest, MutexLock)
{
	Mutex mutex {};
	int Num = 0;
	::std::function<void()> func1 = ::std::bind(
		[&]()
		{
			LockGuard<Mutex> lock(mutex);
			for (int i = 0; i < 100000000; ++i)
			{
				Num += 1;
			}
		});
	::std::function<void()> func2 = ::std::bind(
		[&]()
		{
			LockGuard<Mutex> lock(mutex);
			for (int i = 0; i < 100000000; ++i)
			{
				Num -= 1;
			}
		});
	Thread t1(func1);
	Thread t2(func2);
	t1.start();
	t2.start();
	t1.join();
	t2.join();
	sleep(2);
	ASSERT_TRUE(Num == 0);
}

TEST(ConcurrencyTest, MutexTryLock)
{
	Mutex mutex {};
	int Num = 0;
	::std::function<void()> func1 = ::std::bind(
		[&]()
		{
			LockGuard<Mutex> lock(mutex, LockType::TRY);
			for (int i = 0; i < 100000000; ++i)
			{
				Num += 1;
			}
		});
	::std::function<void()> func2 = ::std::bind(
		[&]()
		{
			LockGuard<Mutex> lock(mutex, LockType::TRY);
			for (int i = 0; i < 100000000; ++i)
			{
				Num -= 1;
			}
		});
	Thread t1(func1);
	Thread t2(func2);
	t1.start();
	t2.start();
	t1.join();
	t2.join();
	sleep(2);
	ASSERT_TRUE(Num != 0);
}

TEST(ConcurrencyTest, MutexDeferLock)
{
	Mutex mutex {};
	int Num = 0;
	::std::function<void()> func1 = ::std::bind(
		[&]()
		{
			LockGuard<Mutex> lock(mutex, LockType::DEFER);
			lock.lock();
			for (int i = 0; i < 100000000; ++i)
			{
				Num += 1;
			}
		});
	::std::function<void()> func2 = ::std::bind(
		[&]()
		{
			LockGuard<Mutex> lock(mutex, LockType::DEFER);
			lock.lock();
			for (int i = 0; i < 100000000; ++i)
			{
				Num -= 1;
			}
		});
	Thread t1(func1);
	Thread t2(func2);
	t1.start();
	t2.start();
	t1.join();
	t2.join();
	sleep(2);
	ASSERT_TRUE(Num == 0);
}

TEST(ConcurrencyTest, CountDownLatchTest)
{
	CountDownLatch latch {5};
	::std::function<void()> func = ::std::bind(
		[&]()
		{
			latch.countDown();
		});
	Thread t1(func);
	Thread t2(func);
	Thread t3(func);
	Thread t4(func);
	Thread t5(func);
	t1.start();
	t2.start();
	t3.start();
	t4.start();
	t5.start();
	latch.wait();
	::printf("CountDownLatch count down = 0\n");
}

TEST(ConcurrencyTest, ConditionVariableTest)
{
	::std::vector<int> vec {};
	vec.reserve(1000);
	Mutex mutex {};
	ConditionVariable notFull {};
	ConditionVariable notEmpty {};
	::std::function<void()> produce = ::std::bind(
		[&]()
		{
			while (true)
			{
				sleepS(1);
				LockGuard<Mutex> lock(mutex);
				while (vec.size() >= 1000)
				{
					notFull.wait(mutex);
				}
				vec.emplace_back(1);
				::printf("%u-produce:%lu\n", currentTid(), vec.size());
				notEmpty.notifyAll();
			}
		});
	::std::function<void()> consume = ::std::bind(
		[&]()
		{
			while (true)
			{
				sleepS(1);
				LockGuard<Mutex> lock(mutex);
				while (vec.empty())
				{
					notEmpty.wait(mutex);
				}
				vec.pop_back();
				::printf("%u-consume:%lu\n", currentTid(), vec.size());
				notFull.notifyAll();
			}
		});
	Thread t1(produce, "t1");
	Thread t2(produce, "t2");
	Thread t3(consume, "t3");
	Thread t4(consume, "t4");
	t1.start();
	t2.start();
	t3.start();
	t4.start();
	t1.join();
	t2.join();
	t3.join();
	t4.join();
	::printf("------%lu", vec.size());
}

int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}