//
// Created by YangGuang on 2022/4/2.
//

#include <gtest/gtest.h>

#include <algorithm>
#include <cmath>
#include "nets/base/ByteStream.h"

using namespace nets::base;

TEST(ByteStreamTest, Append)
{
	ByteStream stream { 1024 };
	ASSERT_EQ(stream.writerIndex(), 0U);
	stream << "abc";
	ASSERT_EQ(stream.writerIndex(), 3U);
	int8_t i8 = 8;
	stream << i8 << &i8 << 12345678U << NAN << 123123.5123123F << 123123123.12312312 << "我爱你 I love you";
	::printf("%u\n", stream.writerIndex());
	::printf("%u\n", stream.writeableBytes());
}

TEST(ByteStreamTest, AppendBuffer)
{
	ByteStream stream { 1024 };
	ASSERT_EQ(stream.writerIndex(), 0U);
	stream << "abc";
	ASSERT_EQ(stream.writerIndex(), 3U);
	ByteStream stream2 { 1024 };
	stream2 << "abc";
	stream << stream2;
	ASSERT_EQ(stream.writerIndex(), 6U);
	ASSERT_EQ(stream.writeableBytes(), 1024U - 6U);
}

int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}