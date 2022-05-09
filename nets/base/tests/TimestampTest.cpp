//
// Created by YangGuang on 2022/5/2.
//

#include <gtest/gtest.h>

#include <sys/time.h>
#include "nets/base/Timestamp.h"

using namespace nets::base;

TEST(TimestampTest, BasicUse)
{
	Timestamp now(Timestamp::now());
	::time_t tmp = now.secondsSinceEpoch();
	Timestamp now2 = ::std::move(now);
	ASSERT_EQ(now2.secondsSinceEpoch(), tmp);
	ASSERT_EQ(now.secondsSinceEpoch(), 0);
}

int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
