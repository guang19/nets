//
// Created by YangGuang on 2022/4/2.
//

#include <gtest/gtest.h>

#include <algorithm>
#include <cmath>
#include "nets/base/log/LogBuffer.h"

using namespace nets::base;

TEST(LogBufferTest, Baseic)
{
	LogBuffer logBuffer;
	ASSERT_EQ(logBuffer.writerIndex(), 0U);
}

TEST(LogBufferTest, Append)
{
	LogBuffer logBuffer;
	ASSERT_EQ(logBuffer.writerIndex(), 0U);
	logBuffer << "abc";
	ASSERT_EQ(logBuffer.writerIndex(), 3U);
	ASSERT_STREQ(logBuffer.buffer(), "abc");
	printf("%s\n", logBuffer.buffer());
	printf("=============================================\n");
	int8_t i8 = 8;
	int n = 5;
	logBuffer << i8 << 12345678U << NAN << 123123.5123123F << 123123123.12312312 << "我爱你 I love you" << &n;
	printf("%p\n", &n);
	printf("%s\n", logBuffer.buffer());
	printf("%u\n", logBuffer.writerIndex());
	printf("%u\n", logBuffer.writeableBytes());
}

TEST(LogBufferTest, AppendBuffer)
{
	LogBuffer logBuffer;
	ASSERT_EQ(logBuffer.writerIndex(), 0U);
	logBuffer << "abc";
	ASSERT_EQ(logBuffer.writerIndex(), 3U);
	LogBuffer logBuffer2;
	logBuffer2 << "abc";
	logBuffer << logBuffer2;
	printf("%s\n", logBuffer.buffer());
	printf("%u\n", logBuffer.writerIndex());
	printf("%u\n", logBuffer.writeableBytes());
}

int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}