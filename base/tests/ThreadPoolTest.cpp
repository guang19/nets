//
// Created by guang19 on 2022/1/3.
//

#include <functional>
#include <gtest/gtest.h>
#include <iostream>
#include "base/threadpool/ThreadPool.h"

TEST(ThreadPoolTest, ConstructException)
{
	ASSERT_THROW(nets::base::ThreadPool(1,0), std::invalid_argument);
}

TEST(ThreadPoolTest, ExecuteTask)
{
	nets::base::ThreadPool* threadPool = new nets::base::ThreadPool(1,1);
	threadPool->init();
	ASSERT_TRUE(threadPool->execute([]()
	{
		std::cout << "hello threadpool" << std::endl;
	}));
	ASSERT_TRUE(threadPool->execute([](int num)
	{
		std::cout << "your enter number is:" << num << std::endl;
	}, 5));
}
}

int main(int argc, char** argv)
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}