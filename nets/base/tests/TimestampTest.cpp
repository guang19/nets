//
// Created by guang19
//

#include <gtest/gtest.h>

#include "nets/base/time/Timestamp.h"

using namespace nets::base;

TEST(TimestampTest, Compare)
{
    Timestamp start(Timestamp::now());
    ::sleep(1);
    Timestamp end(Timestamp::now());
    ASSERT_GE(end, start);
    ASSERT_GE((end.secondsSinceEpoch() - start.secondsSinceEpoch()), 1);
    ::printf("%ld\n", end.secondsSinceEpoch() - start.secondsSinceEpoch());
    ::printf("%ld\n", end.microsPartOfTimestamp() - start.microsPartOfTimestamp());
}

TEST(TimestampTest, PlusTest)
{
    Timestamp now(Timestamp::now());
    ::printf("%ld\n", now.secondsSinceEpoch());
    Timestamp after = now.plusSeconds(3);
    ASSERT_EQ((after.secondsSinceEpoch() - now.secondsSinceEpoch()), 3);
    ::printf("%ld\n", after.secondsSinceEpoch());
}

TEST(TimestampTest, MinusTest)
{
    Timestamp start(Timestamp::now());
    ::sleep(1);
    Timestamp end(Timestamp::now());
    ::printf("%ld\n", start.secondsSinceEpoch());
    ::printf("%ld\n", end.secondsSinceEpoch());
    Timestamp diff = end.minusMicroseconds(start.microsecondsSinceEpoch());
    ASSERT_GE(diff.secondsSinceEpoch(), 1);
    ASSERT_GE(diff.millisecondsSinceEpoch(), 1000);
    ASSERT_GE(diff.microsecondsSinceEpoch(), 1000000);
    ::printf("%ld\n", diff.microsPartOfTimestamp());
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}