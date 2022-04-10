//
// Created by YangGuang on 2022/4/2.
//

#include <gtest/gtest.h>
#include "base/log/LogBuffer.h"

using namespace nets::base;

TEST(LogBufferTest, Baseic)
{
	LogBuffer logBuffer(24U);
	::std::cout << logBuffer.toString() << ::std::endl;
	ASSERT_EQ(logBuffer.length(), 0U);
	ASSERT_EQ(logBuffer.available(), 24U);
}

TEST(LogBufferTest, Append)
{
	LogBuffer logBuffer(4U);
	ASSERT_EQ(logBuffer.length(), 0U);
	ASSERT_EQ(logBuffer.available(), 4U);
	logBuffer << "abc";
	ASSERT_EQ(logBuffer.length(), 3U);
	ASSERT_EQ(logBuffer.available(), 1U);
	ASSERT_STREQ(logBuffer.toString().c_str(), "abc");
	ASSERT_STREQ(logBuffer.getBuffer(), "abc");
	::std::cout << logBuffer.toString() << ::std::endl;
	logBuffer.clear();
	ASSERT_EQ(logBuffer.length(), 0U);
	ASSERT_EQ(logBuffer.available(), 4U);
	::std::cout << "=============================================\n";
	logBuffer << 12345678U << '\n';
	logBuffer << 15.2 << '\n';
	::std::cout << logBuffer.toString();
	::std::cout << logBuffer.getBuffer();
	::std::cout << logBuffer.length() << ::std::endl;
	::std::cout << logBuffer.available() << ::std::endl;
}

TEST(LogBufferTest, AppendBuffer)
{
	LogBuffer logBuffer(4U);
	ASSERT_EQ(logBuffer.length(), 0U);
	ASSERT_EQ(logBuffer.available(), 4U);
	logBuffer << "abc";
	ASSERT_EQ(logBuffer.length(), 3U);
	ASSERT_EQ(logBuffer.available(), 1U);
	LogBuffer logBuffer2(4U);
	logBuffer2 << "abc\n";
	logBuffer << logBuffer2;
	::std::cout << logBuffer.toString() << ::std::endl;
	::std::cout << logBuffer.length() << ::std::endl;
	::std::cout << logBuffer.available() << ::std::endl;
}

TEST(LogBufferTest, Reallocate)
{
	LogBuffer logBuffer(4U);
	ASSERT_EQ(logBuffer.length(), 0U);
	ASSERT_EQ(logBuffer.available(), 4U);
	const char* str = "abcde";
	logBuffer << str;
	logBuffer << '\n';
	ASSERT_EQ(logBuffer.length(), 6U);
//	ASSERT_EQ(logBuffer.available(), 4U);
	::std::cout << logBuffer.available() << ::std::endl;
	ASSERT_STREQ(logBuffer.toString().c_str(), "abcde\n");
	ASSERT_STREQ(logBuffer.getBuffer(), "abcde\n");
	::std::cout << logBuffer.toString();
	logBuffer.clear();
	ASSERT_EQ(logBuffer.length(), 0U);
	::std::cout << logBuffer.available() << ::std::endl;
}

int main(int argc, char** argv)
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}