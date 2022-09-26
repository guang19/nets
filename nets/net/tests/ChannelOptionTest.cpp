//
// Created by guang19
//

#include <gtest/gtest.h>

#include <any>
#include <map>
#include <set>
#include <unordered_map>

#include "nets/net/core/ChannelOption.h"

using namespace nets::net;

using ValueType = ::std::variant<bool, int, SockLinger>;

TEST(ChannelOptionTest, BasicUse)
{
    ::std::unordered_map<int, ValueType> map;
    map[0] = false;
    map[0] = true;
    map[1] = 5;
    ::std::for_each(map.begin(), map.end(),
                    [&](const auto& item)
                    {
                        if (item.first == 0)
                        {
                            printf("map[0] = %d\n", ::std::get<bool>(item.second));
                        }
                        else if (item.first == 1)
                        {
                            printf("map[1] = %d\n", ::std::get<int>(item.second));
                        }
                    });
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}