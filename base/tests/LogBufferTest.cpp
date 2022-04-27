//
// Created by YangGuang on 2022/4/2.
//

#include <gtest/gtest.h>

#include <algorithm>
#include <cmath>
#include "base/log/LogBuffer.h"

using namespace nets::base;

TEST(LogBufferTest, T)
{
	LogBuffer logBuffer;
	::std::cout << logBuffer.getBuffer() << '\n';
	::std::cout << logBuffer.length() << '\n';
	::std::cout << logBuffer.available();
}

TEST(LogBufferTest, Baseic)
{
	LogBuffer logBuffer;
	::std::cout << logBuffer.getBuffer() << ::std::endl;
	ASSERT_EQ(logBuffer.length(), 0U);
	ASSERT_EQ(logBuffer.available(), 65536U);
}

TEST(LogBufferTest, Append)
{
	LogBuffer logBuffer;
	ASSERT_EQ(logBuffer.length(), 0U);
	ASSERT_EQ(logBuffer.available(), 65536U);
	logBuffer << "abc";
	ASSERT_EQ(logBuffer.length(), 3U);
	ASSERT_EQ(logBuffer.available(),  65536U - 3);
	ASSERT_STREQ(logBuffer.getBuffer(), "abc");
	::std::cout << logBuffer.getBuffer() << ::std::endl;
	::std::cout << "=============================================\n";
	int n = 5;
	logBuffer << 12345678U << NAN << 123123.5123123F << 123123123.12312312 << "我爱你 I love you" << &n << '\n';
	::std::cout << &n << '\n';
	::std::cout << logBuffer.getBuffer();
	::std::cout << logBuffer.length() << ::std::endl;
	::std::cout << logBuffer.available() << ::std::endl;
}

TEST(LogBufferTest, AppendBuffer)
{
	LogBuffer logBuffer;
	ASSERT_EQ(logBuffer.length(), 0U);
	ASSERT_EQ(logBuffer.available(), 65536U);
	logBuffer << "abc";
	ASSERT_EQ(logBuffer.length(), 3U);
	ASSERT_EQ(logBuffer.available(), 65536U - 3);
	LogBuffer logBuffer2;
	logBuffer2 << "abc";
	logBuffer << logBuffer2 << '\n';
	::std::cout << logBuffer.getBuffer();
	::std::cout << logBuffer.length() << ::std::endl;
	::std::cout << logBuffer.available() << ::std::endl;
}

int main(int argc, char** argv)
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}