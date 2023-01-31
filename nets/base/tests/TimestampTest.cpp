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

// @brief Test for Timestamp

#include <gtest/gtest.h>

#include "nets/base/Timestamp.h"

using namespace nets;

TEST(TimestampTest, Compare)
{
    Timestamp start(Timestamp::now());
    ::sleep(1);
    Timestamp end(Timestamp::now());
    EXPECT_GE(end, start);
    EXPECT_GE((end.secondsSinceEpoch() - start.secondsSinceEpoch()), 1);
    ::printf("%ld\n", end.secondsSinceEpoch() - start.secondsSinceEpoch());
    ::printf("%ld\n", end.microsPartOfTimestamp() - start.microsPartOfTimestamp());
}

TEST(TimestampTest, PlusTest)
{
    Timestamp now(Timestamp::now());
    ::printf("%ld\n", now.secondsSinceEpoch());
    Timestamp after = now.plusSeconds(3);
    EXPECT_EQ((after.secondsSinceEpoch() - now.secondsSinceEpoch()), 3);
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
    EXPECT_GE(diff.secondsSinceEpoch(), 1);
    EXPECT_GE(diff.millisecondsSinceEpoch(), 1000);
    EXPECT_GE(diff.microsecondsSinceEpoch(), 1000000);
    ::printf("%ld\n", diff.microsPartOfTimestamp());
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}