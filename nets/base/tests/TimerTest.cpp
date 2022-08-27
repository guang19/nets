//
// Created by guang19
//

#include <gtest/gtest.h>

#include <map>

#include "nets/base/timer/Timer.h"

using namespace nets::base;

TEST(TimerTest, CopyMove)
{
    Timestamp now(Timestamp::now());
    Timer timer1(now.plusSeconds(3));
    Timer timer2(::std::move(timer1));
    ASSERT_EQ(timer1.id(), -1);
    ASSERT_EQ(timer1.expiredTime().secondsSinceEpoch(), 0);
    ASSERT_EQ(timer2.id(), 0);
    ASSERT_EQ(timer2.expiredTime(), now.plusSeconds(3));
}

TEST(TimerTest, MultiMapStore)
{
    Timer timer1(Timestamp::now().plusSeconds(4));
    Timer timer2(Timestamp::now().plusSeconds(3));
    Timer timer3(Timestamp::now().plusSeconds(2));
    Timer timer4(Timestamp::now().plusSeconds(2));
    ::std::multimap<Timestamp, Timer> map {};
    map.insert(::std::make_pair(timer1.expiredTime(), ::std::move(timer1)));
    map.insert(::std::make_pair(timer2.expiredTime(), ::std::move(timer2)));
    map.insert(::std::make_pair(timer3.expiredTime(), ::std::move(timer3)));
    map.insert(::std::make_pair(timer4.expiredTime(), ::std::move(timer4)));
    for (const auto& it : map)
    {
        printf("%ld\t", it.first.secondsSinceEpoch());
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}