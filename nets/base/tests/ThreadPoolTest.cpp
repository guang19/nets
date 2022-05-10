//
// Created by guang19 on 2022/1/3.
//

#include <gtest/gtest.h>

#include <functional>

#include "nets/base/concurrency/ThreadPool.h"

using namespace nets::base;

class ThreadPoolTest : public testing::Test
{
public:
	// Sets up the test fixture.
	void SetUp() override
	{
		// set parameters to be smaller, you can observe the result of execute more intuitively
		threadPool = new ThreadPool(16, 24, 30000, 5);
		threadPool->init();
	}

	// Tears down the test fixture.
	void TearDown() override
	{
		delete threadPool;
		threadPool = nullptr;
	}

protected:
	ThreadPool* threadPool;
};

TEST_F(ThreadPoolTest, ConstructParameter)
{
	ThreadPool(1, 0);
	ThreadPool(0, 0);
}

TEST_F(ThreadPoolTest, ExecuteTask)
{
	//	ASSERT_TRUE(threadPool->execute(
	//		[]() -> bool
	//		{
	//			::printf("hello thread pool");
	//			return true;
	//		}));
	//	ASSERT_TRUE(threadPool->execute(
	//		[](int num) -> bool
	//		{
	//			::printf("your enter number is: %d\n", num);
	//			return true;
	//		},
	//		5));
}

TEST_F(ThreadPoolTest, ExecuteTaskLimit)
{
	for (int i = 0; i < 100; ++i)
	{
		threadPool->execute(
			[&]()
			{
				if (i == 5)
				{
					throw ::std::invalid_argument("haha");
				}
				for (int j = 0; j < 10000; ++j)
				{
					::printf("your enter number is: %d, threadId:%d\n", j, currentTid());
				}
			});
	}
	sleepS(3);
}

TEST_F(ThreadPoolTest, SubmitHasRetval)
{
	::std::function<int32_t()> f = []() -> int32_t
	{
		printf("%s\n", currentThreadName());
		return 5;
	};
	auto future1 = threadPool->submit(f);
	future1.wait();
	ASSERT_EQ(future1.get(), 5);
	auto future2 = threadPool->submit(f);
	future2.wait();
	ASSERT_EQ(future2.get(), 5);
}

TEST_F(ThreadPoolTest, SubmitNoRetval)
{
	::std::function<void()> f = []()
	{
		printf("%s\n", currentThreadName());
	};
	auto future1 = threadPool->submit(f);
	future1.wait();
	future1.get();
	auto future2 = threadPool->submit(f);
	future2.wait();
	future2.get();
}

int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}