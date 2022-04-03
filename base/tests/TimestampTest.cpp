//
// Created by YangGuang on 2022/4/2.
//

#include <gtest/gtest.h>
#include "base/log/Timestamp.h"

using namespace nets::base;

TEST(TimestampTest, Valid)
{
	Timestamp t1;
	ASSERT_FALSE(t1);
	Timestamp t2(1);
	ASSERT_TRUE(t2);
}

TEST(TimestampTest, Operator)
{
	Timestamp t1(Timestamp::now());
	::std::cout << "t1: " << t1.formatTime() << ::std::endl;
	Timestamp t2;
	ASSERT_FALSE(t2);
	::std::cout << "t2: " << t2.formatTime() << ::std::endl;
	t2 = t1;
	::std::cout << "after new t2: " << t2.formatTime() << ::std::endl;
	ASSERT_EQ(t1, t2);
}

TEST(TimestampTest, ForMatter)
{
	Timestamp t1 = Timestamp::now();
	::std::cout << "default formatter: " << t1.formatTime() << ::std::endl;
	::std::cout << "default formatter with micros: " << t1.formatTime(true) << ::std::endl;
}

TEST(TimestampTest, FromUnixTime)
{
	Timestamp t1 = Timestamp::now();
	::std::time_t tm;
	time(&tm);
	::std::cout << "formatter: " << t1.formatTime(true) << ::std::endl;
	Timestamp t2 = Timestamp::fromUnixTime(tm);
	::std::cout << "formatter: " << t2.formatTime(true) << ::std::endl;
}

TEST(TimestampTest, PlusSeconds)
{
	Timestamp t1 = Timestamp::now();
	::std::cout << "t1: " << t1.formatTime(true) << ::std::endl;
	::std::cout << "plus seconds: " << t1.plusSeconds(2).formatTime(true) << ::std::endl;
	::std::cout << "sub seconds: " << t1.plusSeconds(-5).formatTime(true) << ::std::endl;
}

TEST(TimestampTest, PlusMinutes)
{
	Timestamp t1 = Timestamp::now();
	::std::cout << "t1: " << t1.formatTime(true) << ::std::endl;
	::std::cout << "plus minutes: " << t1.plusMinutes(2).formatTime(true) << ::std::endl;
	::std::cout << "sub: minutes: " << t1.plusMinutes(-5).formatTime(true) << ::std::endl;
}

TEST(TimestampTest, PlusHours)
{
	Timestamp t1 = Timestamp::now();
	::std::cout << "t1: " << t1.formatTime(true) << ::std::endl;
	::std::cout << "plus hours: " << t1.plusHours(2).formatTime(true) << ::std::endl;
	::std::cout << "sub: hours: " << t1.plusHours(-2).formatTime(true) << ::std::endl;
}

TEST(TimestampTest, PlusDays)
{
	Timestamp t1 = Timestamp::now();
	::std::cout << "t1: " << t1.formatTime(true) << ::std::endl;
	::std::cout << "plus days: " << t1.plusDays(2).formatTime(true) << ::std::endl;
	::std::cout << "sub: days: " << t1.plusDays(-2).formatTime(true) << ::std::endl;
}

int main(int argc, char** argv)
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}