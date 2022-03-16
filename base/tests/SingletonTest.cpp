//
// Created by guang19 on 2022/1/4.
//

#include <gtest/gtest.h>
#include <iostream>
#include <string>
#include <thread>
#include "base/Singleton.hpp"

class Clazz
{
	public:
		Clazz() : name_()
		{
			std::cout << "default construct" << std::endl;
		}

		Clazz(const std::string &s) : name_(s)
		{
			std::cout << "const reference copy construct" << std::endl;
		}

		Clazz(const std::string &&s) : name_(s)
		{
			std::cout << "rvalue reference construct " << std::endl;
		}

		~Clazz()
		{
			std::cout << "Clazz Destruct " << name_ << std::endl;
		}

		void print()
		{
			std::cout << "Clazz = " << name_ << " thread id: " << std::this_thread::get_id() << " " << this
					  << std::endl;
		}

	private:
		std::string name_;
};

TEST(SingletonAddr, EQ)
{
	Clazz *cptr1 = nets::base::Singleton<Clazz>::getInstance();
	Clazz *cptr2 = nets::base::Singleton<Clazz>::getInstance("cptr2");
	Clazz *cptr3 = nets::base::Singleton<Clazz>::getInstance("cptr3");
	std::cout << "SingletonAddr: " << cptr1 << std::endl;
	EXPECT_EQ(cptr1, cptr2);
	EXPECT_EQ(cptr2, cptr3);
	ASSERT_EQ(cptr1, cptr2);
	ASSERT_EQ(cptr2, cptr3);
}

TEST(SingletonAddr, MultiThread)
{
    Clazz* cptr1 = nullptr;
	Clazz* cptr2 = nullptr;
	Clazz* cptr3 = nullptr;
    std::thread t1([&] {
		cptr1 = nets::base::Singleton<Clazz>::getInstance("cp1");
    });

    std::thread t2([&] {
		cptr2 = nets::base::Singleton<Clazz>::getInstance("cp2");
    });
	std::thread t3([&] {
		cptr3 = nets::base::Singleton<Clazz>::getInstance("cp3");
	});
	t1.join();
	t2.join();
	t3.join();
	EXPECT_EQ(cptr1, cptr2);
	EXPECT_EQ(cptr2, cptr3);
	ASSERT_EQ(cptr1, cptr2);
	ASSERT_EQ(cptr2, cptr3);
}

int main(int argc, char **argv)
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
