//
// Created by YangGuang on 2022/4/30.
//

#include <gtest/gtest.h>

#include "nets/base/ThreadHelper.h"

using namespace nets::base;

TEST(ThreadHelperTest, ThreadName)
{
	ASSERT_TRUE((strcmp("Main", currentThreadName()) == 0));
	setThreadName(pthread_self(), "MainThread");
	char name[ThreadNameMaxLength] = {0};
	getThreadName(pthread_self(), name, ThreadNameMaxLength);
	ASSERT_TRUE((strcmp("MainThread", name) == 0));
	ASSERT_TRUE((strcmp(name, currentThreadName()) == 0));
}

TEST(ThreadHelperTest, Tid)
{
	ASSERT_TRUE(isMainThread());
	ASSERT_EQ(currentTid(), ::getpid());
}

int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}