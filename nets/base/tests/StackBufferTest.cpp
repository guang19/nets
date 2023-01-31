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

// @brief Test for StackBuffer

#include <gtest/gtest.h>

#include "nets/base/StackBuffer.h"

using namespace nets;

constexpr SizeType kTestStackBufferSize = 1024;

TEST(StackBufferTest, BasicUse)
{
    StackBuffer<kTestStackBufferSize> buffer1 {};
    buffer1.writeBytes("abcd");
    EXPECT_EQ(buffer1.writableBytes(), kTestStackBufferSize - strlen("abcd"));
    StackBuffer<kTestStackBufferSize> buffer2(buffer1);
    EXPECT_EQ(buffer1.length(), buffer2.length());
    EXPECT_EQ(buffer1.writableBytes(), buffer2.writableBytes());
    EXPECT_STREQ(buffer1.array(), buffer2.array());
}

TEST(StackBufferTest, Move)
{
    StackBuffer<kTestStackBufferSize> buffer1 {};
    buffer1.writeBytes("abcd");
    StackBuffer<kTestStackBufferSize> buffer2(::std::move(buffer1));
    EXPECT_EQ(buffer1.length(), 0U);
    EXPECT_EQ(buffer1.writableBytes(), kTestStackBufferSize);
}

TEST(StackBufferTest, Swap)
{
    StackBuffer<kTestStackBufferSize> buffer1 {};
    buffer1.writeBytes("abcd");
    EXPECT_EQ(buffer1.writableBytes(), kTestStackBufferSize - strlen("abcd"));
    StackBuffer<kTestStackBufferSize> buffer2 {};
    buffer2.writeBytes("efg");
    EXPECT_EQ(buffer2.writableBytes(), kTestStackBufferSize - strlen("efg"));
    buffer1.swap(buffer2);
    EXPECT_EQ(buffer1.writableBytes(), kTestStackBufferSize - strlen("efg"));
    EXPECT_EQ(buffer2.writableBytes(), kTestStackBufferSize - strlen("abcd"));
    EXPECT_EQ(buffer1.length(), 3U);
    EXPECT_EQ(buffer2.length(), 4U);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}