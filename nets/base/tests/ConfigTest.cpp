//
// Created by guang19
//

#include <gtest/gtest.h>

#include "nets/base/config/Config.h"
#include "nets/base/config/ConfigReader.h"

using namespace nets::base;

TEST(ConfigTest, BasicUse)
{
    Config::ConfigMap map {{"key1", "value1"}, {"key2", "1"}, {"key3", "true"}};
    Config config1(map);
    Config config2(config1);
    ASSERT_STREQ(config2.getString("key1").c_str(), "value1");
    ASSERT_EQ(config2.getInt("key2"), 1);
    ASSERT_TRUE(config2.getBool("key3"));
    ASSERT_FALSE(config2.getBool("nullkey", false));
    ASSERT_TRUE(config2.getBool("nullkey", true));
}

TEST(ConfigTest, ConfigReader)
{
    Config config = CONFIG_READER->read("/mnt/c/Users/guang19/Projects/nets/nets/base/tests/test.conf");
    ASSERT_FALSE(config.isEmpty());
    ASSERT_STREQ(config.getString("key1").c_str(), "value1");
    ASSERT_STREQ(config.getString("key2").c_str(), "value2");
    ASSERT_EQ(config.getInt("key3"), 3);
    ASSERT_FALSE(config.getBool("key4"));
    ASSERT_TRUE(config.getBool("key5"));
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}