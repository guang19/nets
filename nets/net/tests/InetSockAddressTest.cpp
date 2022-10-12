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

// @brief Test for InetSockAddress

#include <gtest/gtest.h>

#include <arpa/inet.h>

#include "nets/net/core/InetSockAddress.h"

using namespace nets;

TEST(InetSockAddressTest, BasicUse)
{
    InetSockAddress sockAddress1("127.0.0.1", 8080, false);
    ASSERT_EQ(sockAddress1.ipFamily(), AF_INET);
    ASSERT_STREQ(sockAddress1.ip().c_str(), "127.0.0.1");
    ASSERT_EQ(sockAddress1.port(), 8080);
    ASSERT_STREQ(sockAddress1.toString().c_str(), "127.0.0.1:8080");

    InetSockAddress sockAddress2("2a01:198:603:0:396e:4789:8e99:890f", 8080, true);
    ASSERT_EQ(sockAddress2.ipFamily(), AF_INET6);
    ASSERT_STREQ(sockAddress2.ip().c_str(), "2a01:198:603:0:396e:4789:8e99:890f");
    ASSERT_EQ(sockAddress2.port(), 8080);
    ASSERT_STREQ(sockAddress2.toString().c_str(), "[2a01:198:603:0:396e:4789:8e99:890f]:8080");
}

TEST(InetSockAddressTest, CopyAndMove)
{
    InetSockAddress sockAddress1("127.0.0.1", 8080, false);
    ASSERT_EQ(sockAddress1.ipFamily(), AF_INET);
    ASSERT_STREQ(sockAddress1.ip().c_str(), "127.0.0.1");
    ASSERT_EQ(sockAddress1.port(), 8080);
    ASSERT_STREQ(sockAddress1.toString().c_str(), "127.0.0.1:8080");

    InetSockAddress sockAddress2(sockAddress1);
    ASSERT_EQ(sockAddress2.ipFamily(), sockAddress1.ipFamily());
    ASSERT_STREQ(sockAddress2.ip().c_str(), sockAddress1.ip().c_str());
    ASSERT_EQ(sockAddress2.port(), sockAddress1.port());
    ASSERT_STREQ(sockAddress2.toString().c_str(), sockAddress1.toString().c_str());

    InetSockAddress sockAddress3(::std::move(sockAddress2));
    ASSERT_EQ(sockAddress3.ipFamily(), sockAddress1.ipFamily());
    ASSERT_STREQ(sockAddress3.ip().c_str(), sockAddress1.ip().c_str());
    ASSERT_EQ(sockAddress3.port(), sockAddress1.port());
    ASSERT_STREQ(sockAddress3.toString().c_str(), sockAddress1.toString().c_str());

    ASSERT_EQ(sockAddress2.ipFamily(), 0);
    ASSERT_STREQ(sockAddress2.ip().c_str(), "");
    ASSERT_EQ(sockAddress2.port(), 0);
    ASSERT_STREQ(sockAddress2.toString().c_str(), "");
}

TEST(InetSockAddressTest, Valid)
{
    InetSockAddress address1("127.0.0.1", 8080, false);
    ASSERT_FALSE(address1.isInValid());

    InetSockAddress address2;
    ASSERT_TRUE(address2.isInValid());
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}