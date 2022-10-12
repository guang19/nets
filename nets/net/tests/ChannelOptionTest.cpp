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

// @brief Test for ChannelOption

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