//
// Created by guang19 on 2022/8/4.
//

#include <gtest/gtest.h>

#include "nets/base/exception/AbstractException.h"

using namespace nets::base;

TEST(ExceptionTest, Dump)
{
    AbstractException e;
    ::printf("%s", e.stackTrace().c_str());
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}