//
// Created by guang19 on 2022/1/3.
//

#include <gtest/gtest.h>

#include <functional>
#include <iostream>
#include "nets/base/concurrency/ThreadPool.h"

using namespace nets::base;

class ThreadPoolTest : public testing::Test
{
	public:
		// Sets up the test fixture.
		void SetUp() override
		{
			// set parameters to be smaller, you can observe the result of execute more intuitively
			threadPool = new ThreadPool(2, 5, 30000, 2);
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
	ThreadPool(1,0);
	ThreadPool(0,0);
}

TEST_F(ThreadPoolTest, ExecuteTask)
{
	ASSERT_TRUE(threadPool->execute([]() -> bool
	{
		::std::cout << "hello thread pool" << ::std::endl;
		return true;
	}));
	ASSERT_TRUE(threadPool->execute([](int num) -> bool
	{
		::std::cout << "your enter number is:" << num << ::std::endl;
		return true;
	}, 5));
}

TEST_F(ThreadPoolTest, ExecuteTaskLimit)
{
	for (int i = 0; i < 10; ++i)
	{
		threadPool->execute([&]()
		{
			if (i == 5)
			{
				throw;
			}
			for (int j = 0; j < 10000; ++j)
			{
				::printf("your enter number is: %d, threadId:%d\n", j , currentTid());
			}
		});
	}
	sleepS(3);
}

int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}