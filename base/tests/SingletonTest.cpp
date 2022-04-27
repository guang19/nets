//
// Created by guang19 on 2022/1/4.
//

#include <gtest/gtest.h>

#include <string>
#include <thread>
#include "base/Singleton.h"

using namespace nets::base;

DECLARE_SINGLETON_CLASS(Clazz)
{
	DEFINE_SINGLETON(Clazz)
	public:
		explicit Clazz(const ::std::string& name) : name_(name)
		{
		}

		::std::string getName() const
		{
			return name_;
		}
	private:
		::std::string name_ {};

};
INIT_SINGLETON(Clazz)


// 请单独执行每一个TestCase
// 因为 Clazz::getInstance() 被调用一次被初始化后，其他TestCase再调用也是相同的，所以单独执行每个TestCase才能验证每个Case的实际结果

TEST(SingletonAddr, BasicUse)
{
	Clazz* cptr1 = Clazz::getInstance();
	Clazz* cptr2 = Clazz::getInstance();
	Clazz* cptr3 = Clazz::getInstance("a");
	ASSERT_EQ(cptr1, cptr2);
	ASSERT_EQ(cptr2, cptr3);
	::std::cout << "SingletonAddr: " << cptr1 << ' ' << cptr2 << ' ' << cptr3 << ::std::endl;
	ASSERT_EQ(cptr1->getName(), "");
	ASSERT_EQ(cptr2->getName(), "");
	ASSERT_EQ(cptr3->getName(), "");
}

TEST(SingletonAddr, MultiThread)
{
    Clazz* cptr1 = nullptr;
	Clazz* cptr2 = nullptr;
	Clazz* cptr3 = nullptr;
    ::std::thread t1([&] {
		cptr1 = Clazz::getInstance("cp1");
    });
    ::std::thread t2([&] {
		cptr2 = Clazz::getInstance("cp2");
    });
	::std::thread t3([&] {
		cptr3 = Clazz::getInstance("cp3");
	});
	t1.join();
	t2.join();
	t3.join();
	ASSERT_EQ(cptr1, cptr2);
	ASSERT_EQ(cptr2, cptr3);
	ASSERT_EQ(cptr1->getName(), "cp1");
	ASSERT_EQ(cptr2->getName(), "cp1");
	ASSERT_EQ(cptr3->getName(), "cp1");
}

int main(int argc, char **argv)
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
