//
// Created by YangGuang on 2022/4/2.
//

#include <gtest/gtest.h>
#include "base/log/Timestamp.h"

using namespace nets::base;

TEST(Timestamp, Valid)
{
	Timestamp t1;
	ASSERT_FALSE(t1);
	Timestamp t2(1);
	ASSERT_TRUE(t2);
}

TEST(Timestamp, ForMatter)
{
	Timestamp t1 = Timestamp::now();
	::std::string s = t1.formatTime();
	::std::cout << s << ::std::endl;
}

int main(int argc, char** argv)
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}