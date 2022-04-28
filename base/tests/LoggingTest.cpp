//
// Created by YangGuang on 2022/4/1.
//

#include <gtest/gtest.h>

#include <thread>
#include "base/log/Logging.h"

using namespace nets::base;

TEST(LoggingTest, BasicUse)
{
	LOG_TRACE("这是一条trace信息");
	LOG_DEBUG("这是一条debug信息");
	LOG_INFO("这是一条info信息");
	LOG_WARN("这是一条warn信息");
	LOG_ERROR("这是一条error信息");
	LOG_FATAL("这是一条fatal信息");
	::std::thread t1([&]()
					 {
						 LOG_DEBUG("这是一条debug信息");
					 });
	t1.join();
}

TEST(LoggingTest, StreamAPI)
{
	LOGS_TRACE << "这是一条trace信息\n";
	LOGS_DEBUG << "这是一条debug信息\n";
	LOGS_INFO << "这是一条info信息\n";
	LOGS_WARN << "这是一条warn信息\n";
	LOGS_ERROR << "这是一条error信息\n";
	LOGS_FATAL << "这是一条fatal信息\n";
	::std::thread t1([&]()
					 {
						 LOGS_DEBUG << "这是一条debug信息\n";
					 });
	t1.join();
}

int main(int argc, char** argv)
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}