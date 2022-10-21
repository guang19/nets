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

// @brief Test for StringUtils

#include <gtest/gtest.h>

#include "nets/base/StringUtils.h"

using namespace nets;

TEST(StringUtilsTest, Trim)
{
    StringType s1 = " abcd efg";
    StringType s2 = "abcd efg ";
    StringType s3 = " abcd efg ";
    utils::trim(s1);
    utils::trim(s2);
    utils::trim(s3);
    ASSERT_EQ(s1, s2);
    ASSERT_EQ(s2, s3);
    ASSERT_EQ(s1, s3);
}

TEST(StringUtilsTest, CaseInsensitiveEqual)
{
    StringType s1 = " abcdefg";
    StringType s2 = " ABCDEFG";
    ASSERT_TRUE(utils::caseInsensitiveEqual(s1, s2));
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}