//
// Created by guang19 on 2022/1/6.
//

#include <gtest/gtest.h>
#include <atomic>
#include <deque>
#include <thread>
#include "base/threadpool/BoundedBlockingQueue.h"


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
			delete blockingQueue;
			blockingQueue = nullptr;
		}

	protected:
		BoundedBlockingQueue<int32_t>* blockingQueue;
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
        ::std::thread([&]
                    {
                        blockingQueue->put(i);
                    }).detach();
		::std::thread([&]
					{
						int32_t n;
						blockingQueue->take(n);
					}).detach();
    }
	::std::this_thread::sleep_for(::std::chrono::milliseconds(1000));
	ASSERT_EQ(blockingQueue->size(), 0u);
}

TEST_F(BlockingQueueTest, PutTakeConditionVar)
{
	::std::atomic<bool> running(true);
	ASSERT_EQ(blockingQueue->put(1, [&]() -> bool {
		return !running;
	}), true);
	int32_t takeVal = 0;
	ASSERT_EQ(blockingQueue->take(takeVal, [&]() -> bool {
		return !running;
	}), true);
	ASSERT_EQ(takeVal, 1);
	ASSERT_EQ(blockingQueue->put(2, [&]() -> bool {
		return !running;
	}), true);
	running = false;
	ASSERT_EQ(blockingQueue->take(takeVal, [&]() -> bool {
		return !running;
	}), false);
	ASSERT_EQ(takeVal, 1);
}

TEST_F(BlockingQueueTest, PutTimeout)
{
	blockingQueue->put(1);
	blockingQueue->put(2);
	blockingQueue->put(3);
	blockingQueue->put(4);
	blockingQueue->put(5);
	ASSERT_EQ(blockingQueue->size(), 5u);
	int32_t takeVal = 0;
	int64_t start =
			::std::chrono::duration_cast<::std::chrono::milliseconds>(::std::chrono::steady_clock::now().time_since_epoch())
					.count();
	ASSERT_EQ(blockingQueue->put(takeVal, ::std::chrono::milliseconds(3000)), false);
	int64_t end =
			::std::chrono::duration_cast<::std::chrono::milliseconds>(::std::chrono::steady_clock::now().time_since_epoch())
					.count();
	ASSERT_GT(end, start);
	ASSERT_GE(end - start, 3000);
	ASSERT_EQ(takeVal, 0);
}

TEST_F(BlockingQueueTest, TakeTimeout)
{
	int32_t takeVal = 0;
	int64_t start =
			::std::chrono::duration_cast<::std::chrono::milliseconds>(::std::chrono::steady_clock::now().time_since_epoch())
			        .count();
	ASSERT_EQ(blockingQueue->isEmpty(), true);
	ASSERT_EQ(blockingQueue->take(takeVal, ::std::chrono::milliseconds(3000)), false);
	int64_t end =
			::std::chrono::duration_cast<::std::chrono::milliseconds>(::std::chrono::steady_clock::now().time_since_epoch())
					.count();
	ASSERT_GT(end, start);
	ASSERT_GE(end - start, 3000);
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
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}