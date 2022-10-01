//
// Created by guang19
//

#include <gtest/gtest.h>

#include <any>
#include <map>
#include <set>
#include <unordered_map>

#include "nets/net/core/ChannelOption.h"

using namespace nets;

using ValueType = ::std::variant<bool, int, SockLinger>;

TEST(ChannelOptionTest, BasicUse)
{
    ::std::unordered_map<int, ValueType> map1;
    map1[0] = false;
    map1[1] = 5;
    ::std::unordered_map<int, ValueType> map2;
    map2[2] = false;
    map2[3] = true;
    map2[4] = 5;
    map1.insert(map2.begin(), map2.end());
    ::std::for_each(map1.begin(), map1.end(),
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
                        else if (item.first == 2)
                        {
                            printf("map[2] = %d\n", ::std::get<bool>(item.second));
                        }
                        else if (item.first == 3)
                        {
                            printf("map[3] = %d\n", ::std::get<bool>(item.second));
                        }
                        else if (item.first == 4)
                        {
                            printf("map[4] = %d\n", ::std::get<int>(item.second));
                        }
                    });
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}