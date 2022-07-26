//
// Created by guang19 on 2022/7/26.
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
    ChannelOption A(nets::net::NREUSEADDR, true);
    ChannelOption B(nets::net::NBACKLOG, 5);
    ChannelOption C(nets::net::NBACKLOG, 6);
    ASSERT_EQ(::std::any_cast<bool>(A.get()), true);
    ASSERT_EQ(::std::any_cast<int32_t>(B.get()), 5);
    ASSERT_EQ(::std::any_cast<int32_t>(C.get()), 6);
}

TEST(ChannelOptionTest, Equal)
{
    ASSERT_TRUE(NLinger == NLinger);
    ASSERT_FALSE(NLinger == NBackLog);
}

TEST(ChannelOptionTest, CopyMove)
{
    ChannelOption A(nets::net::NREUSEADDR, true);
    ChannelOption B(nets::net::NBACKLOG, 5);
    A = B;
    ASSERT_EQ(A, B);

    A = ::std::move(B);
    ASSERT_FALSE(B.valid());
}

TEST(ChannelOptionTest, GenericStore)
{
    ::std::map<ChannelOption, ChannelOption::ValueType> options {};
    options.insert({NReuseAddr, true});
    options.insert({NReusePort, true});
    options.insert({NKeepAlive, false});
    options.insert({NTcpNoDelay, false});
    options.insert({NLinger, 0});
    options.insert({NBackLog, 1024});
    options.insert({NTcpSendBuf, 1024});
    options.insert({NTcpRecvBuf, 1024});
    options.insert({NUdpSendBuf, 1024});
    options.insert({NUdpRecvBuf, 1024});
    options.insert_or_assign(NUdpRecvBuf, 1025);
    for (auto it = options.begin(); it != options.end(); ++it)
    {
        if (typeid(int32_t) == it->first.get().type())
        {
            printf("int32_t value : %d\n", ::std::get<int32_t>(it->second));
        }
        else if (typeid(bool) == it->first.get().type())
        {
            printf("bool value : %d\n", ::std::get<bool>(it->second));
        }
    }
    ::std::map<ChannelOption, ChannelOption::ValueType> options2 = ::std::move(options);
    for (auto it = options2.begin(); it != options2.end(); ++it)
    {
        if (typeid(int32_t) == it->first.get().type())
        {
            printf("int32_t value : %d\n", ::std::any_cast<int32_t>(it->first.get()));
        }
        else if (typeid(bool) == it->first.get().type())
        {
            printf("bool value : %d\n", ::std::any_cast<bool>(it->first.get()));
        }
    }
    ASSERT_EQ(options.size(), 0U);
    printf("\n");
}

TEST(ChannelOptionTest, MapStore)
{
    ::std::map<ChannelOption, int32_t> options {};
    options.insert({NLinger, 1});
    options.insert({NBackLog, 2});
    options.insert({NTcpRecvBuf, 3});
    options.insert({NTcpSendBuf, 4});
    options.insert({NUdpSendBuf, 5});
    options.insert({NUdpRecvBuf, 6});
    for (auto it = options.begin(); it != options.end(); ++it)
    {
        printf("{value is %d=>%d}\t", ::std::any_cast<int32_t>(it->first.get()), it->second);
    }
    printf("\n");
}

TEST(ChannelOptionTest, HashMapStore)
{
    ::std::unordered_map<ChannelOption, int32_t, ChannelOption::ChannelOptionHasher> options {};
    options.insert({NLinger, 1});
    options.insert({NBackLog, 2});
    options.insert({NTcpRecvBuf, 3});
    options.insert({NTcpSendBuf, 4});
    options.insert({NUdpSendBuf, 5});
    options.insert({NUdpRecvBuf, 6});
    for (auto it = options.begin(); it != options.end(); ++it)
    {
        printf("{value is %d=>%d}\t", ::std::any_cast<int32_t>(it->first.get()), it->second);
    }
    printf("\n");
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}