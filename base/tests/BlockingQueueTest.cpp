//
// Created by guang19 on 2022/1/6.
//

#include <gtest/gtest.h>
#include <atomic>
#include <deque>
#include <iostream>
#include <thread>
#include "base/threadpool/BlockingQueue.hpp"

class BlockingQueueTest : public testing::Test
{
	public:
		// Sets up the test fixture.
		void SetUp()
		{
			blockingQueue = new nets::base::BlockingQueue<int32_t>(5);
		}

		// Tears down the test fixture.
		void TearDown()
		{
			delete blockingQueue;
			blockingQueue = nullptr;
		}

	protected:
		nets::base::BlockingQueue<int32_t>* blockingQueue;
};

TEST_F(BlockingQueueTest, Put_Take)
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

TEST_F(BlockingQueueTest, Put_Take_Predicate)
{

}


int main(int argc, char** argv)
{
//    auto blockingQueue = new nets::base::BlockingQueue<int32_t>(10);
//    std::hash<std::thread::id> hasher;
//    std::atomic<bool> running(true);
//    for (int i = 0 ; i < 5; ++i)
//    {
//        std::thread([&]
//                    {
//                        printf("thread: %zu put...\n", hasher(std::this_thread::get_id()));
//                        blockingQueue->put(i);
//                        printf("thread: %zu put complete\n", hasher(std::this_thread::get_id()));
//                    }).detach();
//    }
//    for (int i = 0 ; i < 6; ++i)
//    {
//        std::thread([&]
//                    {
//                        int32_t n;
//                        blockingQueue->take(n, [&] () -> bool
//                        {
//                            return running;
//                        });
//                        if (i == 4)
//                        {
//                            running = false;
//                        }
//                        printf("thread: %zu take complete : %d\n", hasher(std::this_thread::get_id()), n);
//                    }).detach();
//    }
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}