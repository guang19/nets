//
// Created by YangGuang on 2022/4/30.
//

#include <gtest/gtest.h>

#include <functional>
#include <sys/prctl.h>
#include "nets/base/Thread.h"
#include "nets/base/ThreadHelper.h"

using namespace nets::base;

TEST(ThreadTest, ThreadHelper)
{
	ASSERT_TRUE(isMainThread());
	ASSERT_TRUE((currentTid() == ::getpid()));
	ASSERT_TRUE((currentTid() == ::gettid()));
	ASSERT_TRUE((strcmp("Main", getCurrentThreadName()) == 0));
//	sleepS(5);
//	sleepMillis(2000);
//	sleepMicros(2000000);
//	sleepNanos(5000000000);
}

void* task1()
{
	sleep(2);
	printf("Enter: task1\n");
	char name[16] = { 0 };
	::prctl(PR_GET_NAME, name);
	printf("prctl current name:%s\n", name);
	printf("ThreadHelper:%s\n", getCurrentThreadName());
	return nullptr;
}

void* task2(int arg)
{
	::std::cout << "task(int)\n";
	return nullptr;
}

void* task3(void* arg)
{
	::std::cout << "task(void*)\n";
	return nullptr;
}

TEST(ThreadTest, ThreadStart)
{
	Thread t1(task1, "task1");
	ASSERT_TRUE((t1.getThreadName() == "task1"));
	ASSERT_TRUE((t1.getThreadId() == 0));
	ASSERT_TRUE((t1.getTid() == 0));
	t1.start();
	ASSERT_TRUE(t1.joinable());
	printf("%s\n", t1.getThreadName().c_str());
	printf("%lu\n", t1.getThreadId());;
	printf("%lu\n", t1.getTid());
}

TEST(ThreadTest, ThreadJoin)
{
	Thread t1(task1, "task1");
	ASSERT_TRUE((t1.getThreadName() == "task1"));
	ASSERT_TRUE((t1.getThreadId() == 0));
	ASSERT_TRUE((t1.getTid() == 0));
	t1.start();
	t1.join();
	ASSERT_FALSE(t1.joinable());
	printf("%s\n", t1.getThreadName().c_str());
	printf("%lu\n", t1.getThreadId());;
	printf("%lu\n", t1.getTid());
}

TEST(ThreadTest, ThreadDetach)
{
	Thread t1(task1, "task1");
	ASSERT_TRUE((t1.getThreadName() == "task1"));
	ASSERT_TRUE((t1.getThreadId() == 0));
	ASSERT_TRUE((t1.getTid() == 0));
	t1.start();
	t1.detach();
	ASSERT_FALSE(t1.joinable());
	printf("%s\n", t1.getThreadName().c_str());
	printf("%lu\n", t1.getThreadId());;
	printf("%lu\n", t1.getTid());
}


int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}