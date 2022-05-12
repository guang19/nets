//
// Created by YangGuang on 2022/4/1.
//

#include <gtest/gtest.h>

#include <thread>

#include "nets/base/log/Logging.h"

using namespace nets::base;

TEST(LoggingTest, BasicUse)
{
	LOGS_TRACE << "这是一条trace信息";
	LOGS_DEBUG << "这是一条debug信息";
	LOGS_INFO << "这是一条info信息";
	LOGS_WARN << "这是一条warn信息";
	LOGS_ERROR << "这是一条error信息";
	::std::thread t1(
		[&]()
		{
			LOGS_DEBUG << "这是一条debug信息";
		});
	t1.join();
//	LOGS_FATAL << "这是一条流式fatal信息";
//	::std::this_thread::sleep_for(::std::chrono::milliseconds(500));
}

// change option LOG_WRITER_TYPE to 1（SINGLE_FILE）
TEST(LoggingTest, SingleFile)
{
	LOGS_TRACE << "这是一条trance信息 stream";
	LOGS_DEBUG << "这是一条debug信息 stream";
	LOGS_INFO << "这是一条info信息 stream";
	LOGS_WARN << "这是一条warn信息 stream";
	LOGS_ERROR << "这是一条error信息 stream";
	LOGS_FATAL << "这是一条流式fatal信息 stream";
}

// before execute:
// change LOG_WRITER_TYPE to 2（DAILY_FILE）
// and change the constant "SecondsPerDay(in LogWriter.cpp)" for short intervals
TEST(LoggingTest, DailyFile)
{
	LOGS_TRACE << "这是一条trance信息 stream";
	LOGS_DEBUG << "这是一条debug信息 stream";
	LOGS_INFO << "这是一条info信息 stream";
	LOGS_WARN << "这是一条warn信息 stream";
	LOGS_ERROR << "这是一条error信息 stream";
	LOGS_FATAL << "这是一条fatal信息 stream";
}

// before execute:
// change LOG_WRITER_TYPE to 3（DLOG_FILE_ROLLING_SIZE）
// and change constant "LogFileRollingSize(in LogWriter.cpp)" as small as possible
TEST(LoggingTest, RollingFile)
{
	LOGS_DEBUG << "这是一条足够长的信息这是一条足够长的信息这是一条足够长的信息这是一条足够长的信息这是一条足够长的信息这是"
				  "一条足够长的信息"
				  "这是一条足够长的信息这是一条足够长的信息这是一条足够长的信息这是一条足够长的信息这是一条足够长的信息这是"
				  "一条足够长的信息"
				  "这是一条足够长的信息这是一条足够长的信息这是一条足够长的信息这是一条足够长的信息这是一条足够长的信息这是"
				  "一条足够长的信息";
}

int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}