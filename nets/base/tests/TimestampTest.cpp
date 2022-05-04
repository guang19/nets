//
// Created by YangGuang on 2022/5/2.
//

#include <gtest/gtest.h>

#include <sys/time.h>
#include "nets/base/Timestamp.h"

using namespace nets::base;

TEST(TimestampTest, BasicUse)
{
	struct timeval tmV {};
	::gettimeofday(&tmV, nullptr);
	Timestamp now(Timestamp::now());
	ASSERT_TRUE((now.getSecondsSinceEpoch() - tmV.tv_sec <= 100));
	ASSERT_TRUE((now.getMicroseconds() - tmV.tv_usec <= 100));
}

int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
