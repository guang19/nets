//
// Created by guang19 on 2022/1/6.
//

#include <gtest/gtest.h>

#include <atomic>
#include <deque>
#include "nets/base/Thread.h"
#include "nets/base/Timestamp.h"
#include "nets/base/concurrency/BoundedBlockingQueue.h"

using namespace nets::base;

class BlockingQueueTest : public testing::Test
{
	public:
		// Sets up the test fixture.
		void SetUp() override
		{
			blockingQueue = new BoundedBlockingQueue<int32_t>(5);
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
		BoundedBlockingQueue<int32_t>* blockingQueue { nullptr };
};

TEST_F(BlockingQueueTest, PutTake)
{
	blockingQueue->put(1);
	blockingQueue->put(2);
	blockingQueue->put(3);
	ASSERT_EQ(blockingQueue->size(), 3u);
	int32_t data = 0;
	blockingQueue->take(data);
	ASSERT_EQ(data, 1);
	blockingQueue->take(data);
	ASSERT_EQ(data, 2);
	blockingQueue->take(data);
	ASSERT_EQ(data, 3);
	ASSERT_EQ(blockingQueue->size(), 0u);
}

TEST_F(BlockingQueueTest, PutTakeMultiThread)
{
    for (int i = 0 ; i < 5; ++i)
    {
        Thread t1([&]
                    {
                        blockingQueue->put(i);
                    });
		t1.start();
		t1.detach();
		Thread t2([&]
					{
						int32_t n;
						blockingQueue->take(n);
					});
		t2.start();
		t2.detach();
	}
	sleepS(1);
	ASSERT_EQ(blockingQueue->size(), 0u);
}

TEST_F(BlockingQueueTest, PutTakeConditionVar)
{
	::std::atomic<bool> running(true);
	::std::function<bool ()> func = ::std::bind([&]() -> bool
		{
			return !running;
		});
	ASSERT_EQ(blockingQueue->put(1, func), true);
	int32_t takeVal = 0;
	ASSERT_EQ(blockingQueue->take(takeVal, func), true);
	ASSERT_EQ(takeVal, 1);
	ASSERT_EQ(blockingQueue->put(2, func), true);
	running = false;
	ASSERT_EQ(blockingQueue->take(takeVal, func), false);
	ASSERT_EQ(takeVal, 2);
	ASSERT_TRUE((blockingQueue->size() == 1));
}

TEST_F(BlockingQueueTest, PutTimeout)
{
	blockingQueue->put(1);
	blockingQueue->put(2);
	blockingQueue->put(3);
	blockingQueue->put(4);
	blockingQueue->put(5);
	ASSERT_EQ(blockingQueue->size(), 5U);
	int32_t takeVal = 0;
	int64_t start = Timestamp::now().secondsSinceEpoch();
	ASSERT_EQ(blockingQueue->put(takeVal, 3000L), false);
	int64_t end = Timestamp::now().secondsSinceEpoch();
	ASSERT_GT(end, start);
	ASSERT_GE((end - start), 3);
	ASSERT_EQ(takeVal, 0);
}

TEST_F(BlockingQueueTest, TakeTimeout)
{
	int32_t takeVal = 0;
	int64_t start = Timestamp::now().secondsSinceEpoch();
	ASSERT_EQ(blockingQueue->isEmpty(), true);
	ASSERT_EQ(blockingQueue->take(takeVal, 3000L), false);
	int64_t end = Timestamp::now().secondsSinceEpoch();
	ASSERT_GT(end, start);
	ASSERT_GE((end - start), 3);
	ASSERT_EQ(takeVal, 0);
}

TEST_F(BlockingQueueTest, TryPushPop)
{
	ASSERT_EQ(blockingQueue->tryPush(1), true);
	int32_t takeVal = 0;
	ASSERT_EQ(blockingQueue->tryPop(takeVal), true);
	ASSERT_EQ(takeVal, 1);
}

int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}