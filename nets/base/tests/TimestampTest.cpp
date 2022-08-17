//
// Created by YangGuang on 2022/5/2.
//

#include <gtest/gtest.h>

#include "nets/base/Timestamp.h"

using namespace nets::base;

TEST(TimestampTest, BasicUse)
{
    Timestamp now(Timestamp::now());
    ::time_t tmp = now.secsFromTimestamp();
    Timestamp now2 = ::std::move(now);
    ASSERT_EQ(now2.secsFromTimestamp(), tmp);
    ASSERT_EQ(now.secsFromTimestamp(), 0);
}

TEST(TimestampTest, Compare)
{
    Timestamp start(Timestamp::now());
    ::sleep(1);
    Timestamp end(Timestamp::now());
    ASSERT_GE(end, start);
    ASSERT_GE((end - start).secsFromTimestamp(), 1);
    ::printf("%ld\n", (end - start).secsFromTimestamp());
    ::printf("%u\n", (end - start).microsFromTimestamp());
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
