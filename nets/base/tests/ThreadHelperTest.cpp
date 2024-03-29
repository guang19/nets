// MIT License
//
// Copyright (c) 2022 guang19
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

// @brief Test for ThreadHelper

#include <gtest/gtest.h>

#include "nets/base/ThreadHelper.h"

using namespace nets;

TEST(ThreadHelperTest, ThreadName)
{
    EXPECT_TRUE(0 == (::strcmp("Main", currentThreadName())));
    setCurrentThreadName("MainThread");
    const char* name = currentThreadName();
    EXPECT_TRUE(0 == (::strcmp("MainThread", name)));
    EXPECT_TRUE(0 == (::strcmp(name, currentThreadName())));
}

TEST(ThreadHelperTest, Tid)
{
    EXPECT_TRUE(isMainThread());
    EXPECT_EQ(currentTid(), ::getpid());
}

TEST(ThreadHelperTest, StackTrace)
{
    ::printf("%s\n", stackTrace().c_str());
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}