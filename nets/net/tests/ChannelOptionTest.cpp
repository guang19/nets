//
// Created by guang19
//

#include <gtest/gtest.h>

#include <any>
#include <iostream>
#include <map>
#include <set>
#include <unordered_map>

#include "nets/net/core/ChannelOption.h"

using namespace nets::net;

TEST(ChannelOptionTest, BasicUse)
{
    ChannelOption A(nets::net::NREUSE_ADDR, true);
    ChannelOption B(nets::net::NBACKLOG, 5);
    ChannelOption C(nets::net::NBACKLOG, 6);
    ASSERT_EQ(::std::any_cast<bool>(A.get()), true);
    ASSERT_EQ(::std::any_cast<::int32_t>(B.get()), 5);
    ASSERT_EQ(::std::any_cast<::int32_t>(C.get()), 6);
}

TEST(ChannelOptionTest, Equal)
{
    ASSERT_TRUE(NLinger == NLinger);
    ASSERT_FALSE(NLinger == NBackLog);
}

TEST(ChannelOptionTest, CopyMove)
{
    ChannelOption A(nets::net::NREUSE_ADDR, true);
    ChannelOption B(nets::net::NBACKLOG, 5);
    A = B;
    ASSERT_EQ(A, B);

    A = ::std::move(B);
    ASSERT_FALSE(B.valid());
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}