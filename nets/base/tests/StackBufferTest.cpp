//
// Created by guang19
//

#include <gtest/gtest.h>

#include "nets/base/StackBuffer.h"

using namespace nets;

constexpr SizeType kTestStackBufferSize = 1024;

TEST(StackBufferTest, BasicUse)
{
    StackBuffer<kTestStackBufferSize> buffer1 {};
    buffer1.writeBytes("abcd");
    ASSERT_EQ(buffer1.writableBytes(), kTestStackBufferSize - strlen("abcd"));
    StackBuffer<kTestStackBufferSize> buffer2(buffer1);
    ASSERT_EQ(buffer1.length(), buffer2.length());
    ASSERT_EQ(buffer1.writableBytes(), buffer2.writableBytes());
    ASSERT_STREQ(buffer1.array(), buffer2.array());
}

TEST(StackBufferTest, Move)
{
    StackBuffer<kTestStackBufferSize> buffer1 {};
    buffer1.writeBytes("abcd");
    StackBuffer<kTestStackBufferSize> buffer2(::std::move(buffer1));
    ASSERT_EQ(buffer1.length(), 0U);
    ASSERT_EQ(buffer1.writableBytes(), kTestStackBufferSize);
}

TEST(StackBufferTest, Swap)
{
    StackBuffer<kTestStackBufferSize> buffer1 {};
    buffer1.writeBytes("abcd");
    ASSERT_EQ(buffer1.writableBytes(), kTestStackBufferSize - strlen("abcd"));
    StackBuffer<kTestStackBufferSize> buffer2 {};
    buffer2.writeBytes("efg");
    ASSERT_EQ(buffer2.writableBytes(), kTestStackBufferSize - strlen("efg"));
    buffer1.swap(buffer2);
    ASSERT_EQ(buffer1.writableBytes(), kTestStackBufferSize - strlen("efg"));
    ASSERT_EQ(buffer2.writableBytes(), kTestStackBufferSize - strlen("abcd"));
    ASSERT_EQ(buffer1.length(), 3U);
    ASSERT_EQ(buffer2.length(), 4U);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}