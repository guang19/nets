//
// Created by YangGuang on 2022/5/2.
//

#include <gtest/gtest.h>

#include <thread>
#include <sys/prctl.h>
#include "nets/base/Thread.h"
#include "nets/base/concurrency/Mutex.h"

using namespace nets::base;

int count = 0;

void func1()
{
	count += 1;
	::std::cout << "func1: " << count << '\n';
}

void func2()
{
	count += 1;
	::std::cout << "func2: " << count << '\n';
}

TEST(ConcurrencyTest, BasicUse)
{
	struct timespec timespec {};
	clock_gettime(CLOCK_REALTIME, &timespec);
	::std::cout << timespec.tv_sec << " " << timespec.tv_nsec;
}

int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}