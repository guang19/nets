//
// Created by guang19
//

#include <gtest/gtest.h>

#include "nets/base/ThreadHelper.h"

using namespace nets;

TEST(ThreadHelperTest, ThreadName)
{
    ASSERT_TRUE(0 == (::strcmp("Main", currentThreadName())));
    setCurrentThreadName("MainThread");
    const char* name = currentThreadName();
    ASSERT_TRUE(0 == (::strcmp("MainThread", name)));
    ASSERT_TRUE(0 == (::strcmp(name, currentThreadName())));
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