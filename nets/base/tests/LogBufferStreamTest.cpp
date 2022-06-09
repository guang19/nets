//
// Created by YangGuang on 2022/4/2.
//

#include <gtest/gtest.h>

#include <algorithm>
#include <cmath>

#include "nets/base/log/LogBufferStream.h"

using namespace nets::base;

TEST(LogBufferStreamTest, Append)
{
	LogBufferStream stream {};
	ASSERT_EQ(stream.cbuffer().writerIndex(), 0U);
	stream << "abc";
	ASSERT_EQ(stream.cbuffer().writerIndex(), 3U);
	int8_t i8 = 8;
	stream << i8 << &i8 << 12345678U << NAN << 123123.5123123F << 123123123.12312312 << "我爱你 I love you";
	::printf("%u\n", stream.cbuffer().writerIndex());
	::printf("%u\n", stream.cbuffer().writeableBytes());
}

TEST(ByteStreamTest, AppendBuffer)
{
	LogBufferStream stream {};
	ASSERT_EQ(stream.cbuffer().writerIndex(), 0U);
	stream << "abc";
	ASSERT_EQ(stream.cbuffer().writerIndex(), 3U);
	LogBufferStream stream2 {};
	stream2 << "abc";
	stream << stream2;
	ASSERT_EQ(stream.cbuffer().writerIndex(), 6U);
	ASSERT_EQ(stream.cbuffer().writeableBytes(), DefaultLogBufferLength - 6U);
}

int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}