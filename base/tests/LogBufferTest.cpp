//
// Created by YangGuang on 2022/4/2.
//

#include <gtest/gtest.h>
#include "base/log/LogBuffer.h"

using namespace nets::base;

TEST(LogBufferTest, BaseicUse)
{
	LogBuffer logBuffer(24);
	ASSERT_EQ(logBuffer.length(), (uint32_t)0);
	ASSERT_EQ(logBuffer.available(), (uint32_t)24);
	::std::cout << "logBuffer: " << logBuffer.toString() << ::std::endl;
	logBuffer.append("this is a test", 14);
	ASSERT_EQ(logBuffer.length(), (uint32_t) strlen("this is a test"));
	ASSERT_EQ(logBuffer.available(), (uint32_t)24 - strlen("this is a test"));
	::std::cout << "logBuffer: " << logBuffer.toString() << ::std::endl;
	logBuffer.clear();
	ASSERT_EQ(logBuffer.length(), (uint32_t)0);
	ASSERT_EQ(logBuffer.available(), (uint32_t)24);
	::std::cout << "logBuffer: " << logBuffer.toString() << ::std::endl;
}

int main(int argc, char** argv)
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}