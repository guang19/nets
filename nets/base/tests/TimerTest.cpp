// MIT License
//
// Copyright (c) 2022 guang19
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

// @brief Test for Timer

#include <gtest/gtest.h>

#include <map>

#include "nets/base/timer/Timer.h"

using namespace nets;

TEST(TimerTest, CopyMove)
{
    Timestamp now(Timestamp::now());
    Timer timer1(now.plusSeconds(3), 1, 1, false);
    Timer timer2(::std::move(timer1));
    ASSERT_EQ(timer1.id().key_, -1);
    ASSERT_EQ(timer1.expiredTime().secondsSinceEpoch(), 0);
    ASSERT_EQ(timer2.id().key_, 0);
    ASSERT_EQ(timer2.expiredTime(), now.plusSeconds(3));
}

TEST(TimerTest, MultiMapStore)
{
    Timestamp now(Timestamp::now());
    ::printf("now=%ld\n", now.microsecondsSinceEpoch());

    Timer timer1(now.plusSeconds(1), 1, 1000, false);
    TimerId timerId1 = timer1.id();
    ::printf("%ld\t", timer1.expiredTime().microsecondsSinceEpoch());

    Timer timer2(now.plusSeconds(2), 1, 1000, false);
    TimerId timerId2 = timer2.id();
    ::printf("%ld\t", timer2.expiredTime().microsecondsSinceEpoch());

    Timer timer3(now.plusSeconds(3), 1, 1000, false);
    TimerId timerId3 = timer3.id();
    ::printf("%ld\t", timer3.expiredTime().microsecondsSinceEpoch());

    Timer timer4(now.plusSeconds(3), 1, 1000, false);
    TimerId timerId4 = timer4.id();
    ::printf("%ld\n", timer4.expiredTime().microsecondsSinceEpoch());

    ::std::multimap<Timestamp, Timer> map {};
    map.insert(::std::make_pair(timer1.expiredTime(), ::std::move(timer1)));
    map.insert(::std::make_pair(timer2.expiredTime(), ::std::move(timer2)));
    map.insert(::std::make_pair(timer3.expiredTime(), ::std::move(timer3)));
    map.insert(::std::make_pair(timer4.expiredTime(), ::std::move(timer4)));

    for (const auto& it : map)
    {
        ::printf("%ld\t%ld\n", it.second.id().key_, it.first.microsecondsSinceEpoch());
    }

    ::printf("\n");
    auto fId1 = map.upper_bound(timerId2.value_);
    auto fId2 = map.upper_bound(timerId2.value_);
    ::printf("%ld\t%ld\n", fId1->second.id().key_, fId2->second.id().key_);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}