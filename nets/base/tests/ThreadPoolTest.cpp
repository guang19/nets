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
		threadPool = new ThreadPool(1, 1, 24);
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

TEST_F(ThreadPoolTest, ExecuteTask)
{
	threadPool->shutdown();
	threadPool->execute(
		[]() -> void
		{
			::printf("1===%s\n", currentThreadName());
		});
	threadPool->execute(
		[](int num) -> bool
		{
			try
			{
				::printf("2===%s\n", currentThreadName());
				throw ::std::invalid_argument("ExecuteTask");
			}
			catch (...)
			{
			}
			return true;
		}, 5);
	threadPool->execute(
		[]() -> bool
		{
			try
			{
				::printf("3===%s\n", currentThreadName());
				throw "ExecuteTask";
			}
			catch (...)
			{
			}
			return true;
		});
	::std::this_thread::sleep_for(::std::chrono::milliseconds(2000));
}

TEST_F(ThreadPoolTest, ExecuteTaskThrow)
{
	threadPool->execute(
		[](int num) -> bool
		{
			::printf("1===%s\n", currentThreadName());
			throw ::std::invalid_argument("ExecuteTask");
		}, 5);
}

TEST_F(ThreadPoolTest, ExecuteTaskLimit)
{
	::std::future<void> fa[100];
	for (int i = 0; i < 100; ++i)
	{
		fa[i] = threadPool->submit(
			[&]()
			{
				for (int j = 0; j < 1000; ++j)
				{
					::printf("your enter number is: %d, threadName:%s\n", j, currentThreadName());
				}
			});
	}
	for (int i = 0; i < 100; ++i)
	{
		fa[i].wait();
		try
		{
			fa[i].get();
		}
		catch (::std::future_error& e)
		{
			::printf("%s\n", e.what());
		}
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
	auto future2 = threadPool->submit(::std::move(f));
	future2.wait();
	ASSERT_EQ(future2.get(), 5);
}

TEST_F(ThreadPoolTest, SubmitNoRetval)
{
	::std::function<void()> f = []()
	{
		::printf("%s\n", currentThreadName());
	};
	auto future1 = threadPool->submit(f);
	future1.wait();
	future1.get();
	auto future2 = threadPool->submit(::std::move(f));
	future2.wait();
	future2.get();
}

TEST_F(ThreadPoolTest, SubmitFutureThrow)
{
	::std::function<int32_t()> f1 = []() -> int32_t
	{
		::printf("%s\n", currentThreadName());
		throw ::std::invalid_argument("123123123");
		return 5;
	};

	::std::function<void()> f2 = []()
	{
		::printf("%s\n", currentThreadName());
		throw ::std::invalid_argument("123123123");
	};
	auto future1 = threadPool->submit(f1);
	future1.wait();
	ASSERT_THROW(future1.get(), ::std::exception);
	auto future2 = threadPool->submit(f2);
	future2.wait();
	ASSERT_THROW(future2.get(), ::std::exception);
}

int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}