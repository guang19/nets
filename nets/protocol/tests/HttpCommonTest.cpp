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

// @brief Test for http common

#include <gtest/gtest.h>

#include "nets/protocol/http/HttpCommon.h"

using namespace nets;

TEST(HttpCommonTest, HttpHeader)
{
    ASSERT_STREQ("X-XSS-Protection", httpHeaderToString(HttpHeader::X_XSS_PROTECTION).c_str());
}

TEST(HttpCommonTest, HttpStatusCode)
{
    ASSERT_EQ(404, httpStatusToCode(HttpStatus::NOT_FOUND));
    ASSERT_EQ(200, httpStatusToCode(HttpStatus::OK));
    ASSERT_EQ(500, httpStatusToCode(HttpStatus::INTERNAL_SERVER_ERROR));
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}