//
// Created by guang19 on 2022/1/3.
//

#include <gtest/gtest.h>
#include <functional>
#include <iostream>
#include "base/threadpool/ThreadPool.h"

using namespace nets::base;

class ThreadPoolTest : public testing::Test
{
	public:
		// Sets up the test fixture.
		void SetUp() override
		{
			threadPool = new ThreadPool(2, 2);
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

TEST_F(ThreadPoolTest, ConstructException)
{
	ASSERT_THROW(ThreadPool(1,0), ::std::invalid_argument);
}

TEST_F(ThreadPoolTest, ExecuteTask)
{

	ASSERT_TRUE(threadPool->execute([]()
	{
		::std::cout << "hello threadpool" << ::std::endl;
	}));
	ASSERT_TRUE(threadPool->execute([](int num)
	{
		::std::cout << "your enter number is:" << num << ::std::endl;
	}, 5));

}

int main(int argc, char** argv)
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}