//
// Created by n021949 on 2022/4/28.
//

#include <gtest/gtest.h>

#include "nets/base/CommonMacro.h"

class TBase
{
public:
	void BaseFun() {}
};

class T : public TBase
{

public:
	void TestFun() {}

	void TestFunWithParameter(int) {}
};

DECLARE_HAS_MEMBER_FUNCTION(BaseFun);
DECLARE_HAS_MEMBER_FUNCTION(TestFun);
DECLARE_HAS_MEMBER_FUNCTION(TestFunWithParameter);

TEST(CommonMacroTest, HasMemberFunction)
{
	ASSERT_TRUE(HAS_MEMBER_FUNCTION(T, BaseFun));
	ASSERT_TRUE(HAS_MEMBER_FUNCTION(T, TestFun));
	ASSERT_TRUE((HAS_MEMBER_FUNCTION(T, TestFunWithParameter, int)));
}

int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}