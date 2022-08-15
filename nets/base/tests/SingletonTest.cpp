//
// Created by guang19
//

#include <gtest/gtest.h>

#include <string>
#include <thread>

#include "nets/base/Singleton.h"

using namespace nets::base;

DECLARE_SINGLETON_CLASS(Clazz)
{
    DEFINE_SINGLETON(Clazz);

public:
    Clazz() {}

    explicit Clazz(const ::std::string& name) : name_(name) {}

    ::std::string name() const
    {
        return name_;
    }

private:
    ::std::string name_ {};
};
INIT_SINGLETON(Clazz);

// execute each test-case individually
// because singleton instance has been initialized since getInstance called first time,
// other test-case re-calls are the same singleton instance

TEST(SingletonAddr, BasicUse)
{
    auto cptr1 = Clazz::getInstance();
    auto cptr2 = Clazz::getInstance();
    auto cptr3 = Clazz::getInstance("a");
    ASSERT_EQ(cptr1, cptr2);
    ASSERT_EQ(cptr2, cptr3);
    ::printf("SingletonAddr: %p %p %p\n", cptr1.get(), cptr2.get(), cptr3.get());
    ASSERT_EQ(cptr1->name(), "");
    ASSERT_EQ(cptr2->name(), "");
    ASSERT_EQ(cptr3->name(), "");
}

TEST(SingletonAddr, MultiThread)
{
    ::std::shared_ptr<Clazz> cptr1 = nullptr;
    ::std::shared_ptr<Clazz> cptr2 = nullptr;
    ::std::shared_ptr<Clazz> cptr3 = nullptr;
    ::std::thread t1(
        [&]()
        {
            cptr1 = Clazz::getInstance("cp1");
        });
    ::std::thread t2(
        [&]()
        {
            cptr2 = Clazz::getInstance("cp2");
        });
    ::std::thread t3(
        [&]()
        {
            cptr3 = Clazz::getInstance("cp3");
        });
    t1.join();
    t2.join();
    t3.join();
    ASSERT_EQ(cptr1, cptr2);
    ASSERT_EQ(cptr2, cptr3);
    ASSERT_EQ(cptr1->name(), "cp1");
    ASSERT_EQ(cptr2->name(), "cp1");
    ASSERT_EQ(cptr3->name(), "cp1");
}

DECLARE_SINGLETON_CLASS(Clazz2)
{
    DEFINE_SINGLETON(Clazz2);

public:
    inline void afterInit()
    {
        ::printf("afterInit without parameter\n");
    }
};
INIT_SINGLETON(Clazz2);

TEST(SingletonAddr, AfterInit)
{
    Clazz2::getInstance();
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
