//
// Created by guang19
//

#include <gtest/gtest.h>

#include "nets/base/time/Timestamp.h"

using namespace nets::base;

TEST(TimestampTest, BasicUse)
{
    Timestamp t1(Timestamp::now());
    ::printf("%ld\n", t1.seconds());
    ::printf("%ld\n", t1.microseconds());
    ::printf("%ld\n", ::time(nullptr));
}

TEST(TimestampTest, Compare)
{
    Timestamp start(Timestamp::now());
    ::sleep(1);
    Timestamp end(Timestamp::now());
    ASSERT_GE(end, start);
    ASSERT_GE((end.seconds() - start.seconds()), 1);
    ::printf("%ld\n", end.seconds() - start.seconds());
    ::printf("%ld\n", end.microsPartOfTimestamp() - start.microsPartOfTimestamp());
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}