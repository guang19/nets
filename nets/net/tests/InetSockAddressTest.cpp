//
// Created by guang19 on 2022/5/13.
//

#include <gtest/gtest.h>

#include <arpa/inet.h>

#include "nets/net/core/InetSockAddress.h"

using namespace nets::net;

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

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}