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

// @brief Test for ChannelHandlerPipeline

#include <gtest/gtest.h>

#include "nets/net/core/DatagramChannelHandlerPipeline.h"
#include "nets/net/core/SocketChannelHandler.h"
#include "nets/net/core/DatagramChannelHandler.h"
#include "nets/net/core/SocketChannelHandlerPipeline.h"

using namespace nets;

class TestSocketChannelHandler : public SocketChannelHandler
{
public:
    TestSocketChannelHandler() {}
    explicit TestSocketChannelHandler(const StringType& name) : SocketChannelHandler(name) {}

    ~TestSocketChannelHandler()
    {
        ::printf("~TestSocketChannelHandler\n");
    }
};

TEST(SocketChannelHandlerPipelineTest, AddRemove)
{
    SocketChannelHandlerPipeline pipeline(nullptr);
    auto handler1 = ::std::make_shared<TestSocketChannelHandler>("handler1");
    auto handler2 = ::std::make_shared<TestSocketChannelHandler>("handler2");
    auto handler3 = ::std::make_shared<TestSocketChannelHandler>("handler3");
    auto handler4 = ::std::make_shared<TestSocketChannelHandler>("handler4");
    auto handler5 = ::std::make_shared<TestSocketChannelHandler>("handler5");
    auto handler6 = ::std::make_shared<TestSocketChannelHandler>("handler6");
    auto handler7 = ::std::make_shared<TestSocketChannelHandler>("handler7");
    pipeline.addFirst(handler2);
    pipeline.addFirst(handler1);
    pipeline.addLast(handler3);
    pipeline.addLast(handler4);
    pipeline.addFirst(handler5);
    pipeline.addLast(handler6);
    pipeline.addLast(handler7);
    EXPECT_TRUE(pipeline.remove("handler4") == handler4);
    EXPECT_TRUE(pipeline.removeFirst() == handler5);
    EXPECT_TRUE(pipeline.remove("handler3") == handler3);
    EXPECT_TRUE(pipeline.removeLast() == handler7);
    EXPECT_TRUE(pipeline.removeFirst() == handler1);
    EXPECT_TRUE(pipeline.removeFirst() == handler2);
    EXPECT_TRUE(pipeline.remove(handler6));
    EXPECT_TRUE(pipeline.removeLast() == nullptr);
}

TEST(SocketChannelHandlerPipelineTest, StackOverflow)
{
    SocketChannelHandlerPipeline pipeline(nullptr);
    StringType namePrefix = "handler";
    for (Int32Type i = 0; i < 10000; ++i)
    {
        pipeline.addLast(::std::make_shared<TestSocketChannelHandler>(namePrefix.append(::std::to_string(i))));
    }
}

class TestDatagramChannelHandler : public DatagramChannelHandler
{
public:
    TestDatagramChannelHandler() {}
    explicit TestDatagramChannelHandler(const StringType& name) : DatagramChannelHandler(name) {}
};

TEST(DatagramChannelHandlerPipelineTest, AddRemove)
{
    DatagramChannelHandlerPipeline pipeline(nullptr);
    auto handler1 = ::std::make_shared<TestDatagramChannelHandler>("handler1");
    auto handler2 = ::std::make_shared<TestDatagramChannelHandler>("handler2");
    auto handler3 = ::std::make_shared<TestDatagramChannelHandler>("handler3");
    auto handler4 = ::std::make_shared<TestDatagramChannelHandler>("handler4");
    auto handler5 = ::std::make_shared<TestDatagramChannelHandler>("handler5");
    auto handler6 = ::std::make_shared<TestDatagramChannelHandler>("handler6");
    auto handler7 = ::std::make_shared<TestDatagramChannelHandler>("handler7");
    pipeline.addFirst(handler2);
    pipeline.addFirst(handler1);
    pipeline.addLast(handler3);
    pipeline.addLast(handler4);
    pipeline.addFirst(handler5);
    pipeline.addLast(handler6);
    pipeline.addLast(handler7);
    EXPECT_TRUE(pipeline.remove("handler4") == handler4);
    EXPECT_TRUE(pipeline.removeFirst() == handler5);
    EXPECT_TRUE(pipeline.remove("handler3") == handler3);
    EXPECT_TRUE(pipeline.removeLast() == handler7);
    EXPECT_TRUE(pipeline.removeFirst() == handler1);
    EXPECT_TRUE(pipeline.removeFirst() == handler2);
    EXPECT_TRUE(pipeline.remove(handler6));
    EXPECT_TRUE(pipeline.removeLast() == nullptr);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}