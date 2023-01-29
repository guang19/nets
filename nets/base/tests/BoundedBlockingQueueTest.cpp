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

// @brief Test for BoundedBlockingQueue

#include <gtest/gtest.h>

#include <thread>

#include "nets/base/BoundedBlockingQueue.h"
#include "nets/base/Timestamp.h"

using namespace nets;

class BoundedBlockingQueueTest : public testing::Test
{
public:
    // Sets up the test fixture.
    void SetUp() override
    {
        blockingQueue = new BoundedBlockingQueue<Int32Type>(5);
    }

    // Tears down the test fixture.
    void TearDown() override
    {
        if (blockingQueue != nullptr)
        {
            delete blockingQueue;
            blockingQueue = nullptr;
        }
    }

protected:
    BoundedBlockingQueue<Int32Type>* blockingQueue {nullptr};
};

TEST_F(BoundedBlockingQueueTest, PutTake)
{
    blockingQueue->put(1);
    blockingQueue->put(2);
    blockingQueue->put(3);
    ASSERT_EQ(blockingQueue->size(), 3u);
    Int32Type data = 0;
    blockingQueue->take(data);
    ASSERT_EQ(data, 1);
    blockingQueue->take(data);
    ASSERT_EQ(data, 2);
    blockingQueue->take(data);
    ASSERT_EQ(data, 3);
    ASSERT_EQ(blockingQueue->size(), 0u);
}

TEST_F(BoundedBlockingQueueTest, PutTakeMultiThread)
{
    for (int i = 0; i < 5; ++i)
    {
        ::std::thread t1(
            [&]
            {
                blockingQueue->put(i);
            });
        t1.detach();
        ::std::thread t2(
            [&]
            {
                Int32Type n;
                blockingQueue->take(n);
            });
        t2.detach();
    }
    ::std::this_thread::sleep_for(::std::chrono::milliseconds(1000));
    ASSERT_EQ(blockingQueue->size(), 0u);
}

TEST_F(BoundedBlockingQueueTest, PutTakeConditionVar)
{
    ::std::atomic<bool> running(true);
    ::std::function<bool()> func = [&]() -> bool
    {
        return !running;
    };
    ASSERT_EQ(blockingQueue->put(1, func), true);
    Int32Type takeVal = 0;
    ASSERT_EQ(blockingQueue->take(takeVal, func), true);
    ASSERT_EQ(takeVal, 1);
    ASSERT_EQ(blockingQueue->put(2, func), true);
    running = false;
    ASSERT_EQ(blockingQueue->take(takeVal, func), false);
    ASSERT_EQ(takeVal, 1);
    ASSERT_TRUE(1 == (blockingQueue->size()));
}

TEST_F(BoundedBlockingQueueTest, PutTimeout)
{
    blockingQueue->put(1);
    blockingQueue->put(2);
    blockingQueue->put(3);
    blockingQueue->put(4);
    blockingQueue->put(5);
    ASSERT_EQ(blockingQueue->size(), 5U);
    Int32Type takeVal = 0;
    Int64Type start = Timestamp::now().secondsSinceEpoch();
    ASSERT_EQ(blockingQueue->put(takeVal, 3000L), false);
    Int64Type end = Timestamp::now().secondsSinceEpoch();
    ASSERT_GT(end, start);
    ASSERT_GE((end - start), 3);
    ASSERT_EQ(takeVal, 0);
}

TEST_F(BoundedBlockingQueueTest, TakeTimeout)
{
    Int32Type takeVal = 0;
    Int64Type start = Timestamp::now().secondsSinceEpoch();
    ASSERT_EQ(blockingQueue->isEmpty(), true);
    ASSERT_EQ(blockingQueue->take(takeVal, 3000L), false);
    Int64Type end = Timestamp::now().secondsSinceEpoch();
    ASSERT_GT(end, start);
    ASSERT_GE((end - start), 3);
    ASSERT_EQ(takeVal, 0);
}

TEST_F(BoundedBlockingQueueTest, TryPushPop)
{
    ASSERT_EQ(blockingQueue->tryPush(1), true);
    Int32Type takeVal = 0;
    ASSERT_EQ(blockingQueue->tryPop(takeVal), true);
    ASSERT_EQ(takeVal, 1);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}