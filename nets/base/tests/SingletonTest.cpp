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

// @brief Test for Singleton

#include <gtest/gtest.h>

#include <string>
#include <thread>

#include "nets/base/Singleton.h"
#include "nets/base/Types.h"

using namespace nets;

DECLARE_SINGLETON_CLASS(Clazz)
{
    DEFINE_SINGLETON(Clazz);

public:
    Clazz() {}

    explicit Clazz(const StringType& name) : name_(name) {}

    StringType name() const
    {
        return name_;
    }

private:
    StringType name_ {};
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
    EXPECT_EQ(cptr1, cptr2);
    EXPECT_EQ(cptr2, cptr3);
    ::printf("SingletonAddr: %p %p %p\n", cptr1.get(), cptr2.get(), cptr3.get());
    EXPECT_EQ(cptr1->name(), "");
    EXPECT_EQ(cptr2->name(), "");
    EXPECT_EQ(cptr3->name(), "");
}

DECLARE_SINGLETON_CLASS(Clazz2)
{
    DEFINE_SINGLETON(Clazz2);

public:
    Clazz2() {}

    explicit Clazz2(const StringType& name) : name_(name) {}

    StringType name() const
    {
        return name_;
    }

private:
    StringType name_ {};
};
INIT_SINGLETON(Clazz2);

TEST(SingletonAddr, MultiThread)
{
    ::std::shared_ptr<Clazz2> cptr1 = nullptr;
    ::std::shared_ptr<Clazz2> cptr2 = nullptr;
    ::std::shared_ptr<Clazz2> cptr3 = nullptr;
    ::std::thread t1(
        [&]()
        {
            cptr1 = Clazz2::getInstance("cp1");
        });
    t1.join();
    ::std::thread t2(
        [&]()
        {
            cptr2 = Clazz2::getInstance("cp2");
        });
    ::std::thread t3(
        [&]()
        {
            cptr3 = Clazz2::getInstance("cp3");
        });
    t2.join();
    t3.join();
    EXPECT_EQ(cptr1, cptr2);
    EXPECT_EQ(cptr2, cptr3);
    EXPECT_EQ(cptr1->name(), "cp1");
    EXPECT_EQ(cptr2->name(), "cp1");
    EXPECT_EQ(cptr3->name(), "cp1");
}

DECLARE_SINGLETON_CLASS(Clazz3)
{
    DEFINE_SINGLETON(Clazz3);

public:
    inline void afterInit()
    {
        ::printf("afterInit without parameter\n");
    }
};
INIT_SINGLETON(Clazz3);

TEST(SingletonAddr, AfterInit)
{
    Clazz3::getInstance();
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}