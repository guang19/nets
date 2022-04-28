//
// Created by n021949 on 2022/4/28.
//

#include <gtest/gtest.h>

#include "base/CommonMacro.h"

class TBase
{
	protected:
		void BaseFun()
		{

		}
};

class T : public TBase
{
		DECLARE_HAS_MEMBER_FUNCTION(BaseFun);
		DECLARE_HAS_MEMBER_FUNCTION(TestFun);
	public:
		void TestFun()
		{
		}

		T()
		{
			std::cout << HAS_MEMBER_FUNCTION(T, BaseFun) << '\n';
			std::cout << HAS_MEMBER_FUNCTION(T, TestFun) << '\n';
		}
};



TEST(CommonMacroTest, HasMemberFunction)
{
	T t;
}



int main(int argc, char** argv)
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}