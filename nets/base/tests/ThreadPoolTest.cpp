//
// Created by guang19 on 2022/1/3.
//

#include <gtest/gtest.h>

#include "nets/base/concurrency/ThreadPool.h"
#include "nets/base/ThreadHelper.h"

using namespace nets::base;

class ThreadPoolTest : public testing::Test
{
public:
	// Sets up the test fixture.
	void SetUp() override
	{
		// set parameters to be smaller, you can observe the result of execute more intuitively
		threadPool = new ThreadPool();
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
	threadPool->execute(
		[]() -> void
		{
			::printf("===%s\n", currentThreadName());
		});

	threadPool->execute(
		[](int num) -> bool
		{
			::printf("===%s\n", currentThreadName());
		  throw ::std::invalid_argument("23123123123");
		  return true;
		},
		5);

	threadPool->execute(
		[]() -> bool
		{
			::printf("===%s\n", currentThreadName());
			throw "12312312312313123";
			return true;
		});
}

TEST_F(ThreadPoolTest, ExecuteTaskLimit)
{
	for (int i = 0; i < 10; ++i)
	{
		threadPool->execute(
			[&]()
			{
				for (int j = 0; j < 1000; ++j)
				{
					::printf("your enter number is: %d, threadName:%s\n", j, currentThreadName());
				}
			});
	}
}

TEST_F(ThreadPoolTest, SubmitHasRetval)
{
	::std::function<int32_t()> f = []() -> int32_t
	{
		::printf("%s\n", currentThreadName());
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
		throw ::std::invalid_argument("123123123");
		::printf("%s\n", currentThreadName());
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