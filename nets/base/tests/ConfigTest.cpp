//
// Created by guang19
//

#include <gtest/gtest.h>

#include "nets/base/config/Config.h"

using namespace nets::base;

TEST(ConfigTest, BasicUse)
{
    Config::ConfigMap map {{"key1", "value1"}, {"key2", "1"}, {"key3", "true"}};
    Config config1(map);
    Config config2(config1);
    ASSERT_STREQ(config2.getString("key1").c_str(), "value1");
    ASSERT_EQ(config2.getInt("key2"), 1);
    ASSERT_TRUE(config2.getBool("key3"));
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}