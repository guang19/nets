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

// @brief Test for LogBufferStream

#include <gtest/gtest.h>

#include "nets/base/log/LogBuffer.h"
#include <iostream>

using namespace nets;

TEST(LogBufferStreamTest, Append)
{
    LogBufferStream stream {};
    EXPECT_EQ(stream.buffer().writerIndex(), 0U);
    stream << "abc";
    EXPECT_EQ(stream.buffer().writerIndex(), 3U);
    Int8Type i8 = 8;
    stream << i8 << &i8 << 12345678U << NAN << 123123.5123123F << 123123123.123123123456 << "我爱你 I love you";
    ::printf("%lu\n", stream.buffer().writerIndex());
    ::printf("%lu\n", stream.buffer().writableBytes());
    ::printf("%s\n", stream.buffer().array());
}

TEST(LogBufferStreamTest, AppendBuffer)
{
    LogBufferStream stream {};
    EXPECT_EQ(stream.buffer().writerIndex(), 0U);
    stream << "abc";
    EXPECT_EQ(stream.buffer().writerIndex(), 3U);
    LogBufferStream stream2 {};
    stream2 << "abc";
    stream << stream2;
    EXPECT_EQ(stream.buffer().writerIndex(), 6U);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}