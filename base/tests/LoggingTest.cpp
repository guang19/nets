//
// Created by YangGuang on 2022/4/1.
//

#include <gtest/gtest.h>
#include "base/log/Logging.h"

using namespace nets::base;

TEST(LoggingTest, T)
{
	Logger logger;
	logger.log(LogLevel::TRACE, __FILE__, __FUNCTION__, __LINE__);
}

int main(int argc, char** argv)
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}