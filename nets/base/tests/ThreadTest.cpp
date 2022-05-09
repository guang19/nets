//
// Created by YangGuang on 2022/4/30.
//

#include <gtest/gtest.h>

#include <functional>
#include <sys/prctl.h>
#include "nets/base/ThreadHelper.h"
#include "nets/base/Thread.h"

using namespace nets::base;

TEST(ThreadTest, ThreadHelper)
{
	ASSERT_TRUE(isMainThread());
	ASSERT_TRUE((currentTid() == ::getpid()));
	ASSERT_TRUE((currentTid() == ::gettid()));
	ASSERT_TRUE((strcmp("Main", currentThreadName()) == 0));
//	sleepS(5);
//	sleepMillis(2000);
//	sleepMicros(2000000);
//	sleepNanos(5000000000);
}

void* task1()
{
	sleep(2);
	::printf("Enter: task1\n");
	char name[16] = { 0 };
	::prctl(PR_GET_NAME, name);
	::printf("prctl current name:%s\n", name);
	::printf("ThreadHelper:%s\n", currentThreadName());
	return nullptr;
}

void* task2(int arg)
{
	::printf("task(int)\n");
	return nullptr;
}

void* task3(void* arg)
{
	::printf("task(void*)\n");
	return nullptr;
}

TEST(ThreadTest, ThreadStart)
{
	Thread t1(task1, "task1");
	ASSERT_TRUE((t1.threadName() == "task1"));
	ASSERT_TRUE((t1.threadId() == 0));
	ASSERT_TRUE((t1.tid() == 0));
	t1.start();
	ASSERT_TRUE(t1.joinable());
	::printf("%s\n", t1.threadName().c_str());
	::printf("%lu\n", t1.threadId());;
	::printf("%lu\n", t1.tid());
}

TEST(ThreadTest, ThreadJoin)
{
	Thread t1(task1, "task1");
	ASSERT_TRUE((t1.threadName() == "task1"));
	ASSERT_TRUE((t1.threadId() == 0));
	ASSERT_TRUE((t1.tid() == 0));
	t1.start();
	t1.join();
	ASSERT_FALSE(t1.joinable());
	::printf("%s\n", t1.threadName().c_str());
	::printf("%lu\n", t1.threadId());;
	::printf("%lu\n", t1.tid());
}

TEST(ThreadTest, ThreadDetach)
{
	Thread t1(task1, "task1");
	ASSERT_TRUE((t1.threadName() == "task1"));
	ASSERT_TRUE((t1.threadId() == 0));
	ASSERT_TRUE((t1.tid() == 0));
	t1.start();
	t1.detach();
	ASSERT_FALSE(t1.joinable());
	::printf("%s\n", t1.threadName().c_str());
	::printf("%lu\n", t1.threadId());;
	::printf("%lu\n", t1.tid());
}


int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}