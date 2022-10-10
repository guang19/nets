//
// Created by guang19
//

#include <gtest/gtest.h>

#include "nets/base/log/LogBuffer.h"
#include <iostream>

using namespace nets;

TEST(LogBufferStreamTest, Append)
{
    LogBufferStream stream {};
    ASSERT_EQ(stream.buffer().writerIndex(), 0U);
    stream << "abc";
    ASSERT_EQ(stream.buffer().writerIndex(), 3U);
    ::int8_t i8 = 8;
    stream << i8 << &i8 << 12345678U << NAN << 123123.5123123F << 123123123.123123123456 << "我爱你 I love you";
    ::printf("%lu\n", stream.buffer().writerIndex());
    ::printf("%lu\n", stream.buffer().writableBytes());
    ::printf("%s\n", stream.buffer().array());
}

TEST(LogBufferStreamTest, AppendBuffer)
{
    LogBufferStream stream {};
    ASSERT_EQ(stream.buffer().writerIndex(), 0U);
    stream << "abc";
    ASSERT_EQ(stream.buffer().writerIndex(), 3U);
    LogBufferStream stream2 {};
    stream2 << "abc";
    stream << stream2;
    ASSERT_EQ(stream.buffer().writerIndex(), 6U);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}