//
// Created by YangGuang on 2022/4/1.
//

#include <gtest/gtest.h>

#include "nets/base/log/Logging.h"
#include "nets/base/Thread.h"

using namespace nets::base;

TEST(LoggingTest, BasicUse)
{
	LOG_TRACE("这是一条trace信息");
	LOG_DEBUG("这是一条debug信息");
	LOG_INFO("这是一条info信息");
	LOG_WARN("这是一条warn信息");
	LOG_ERROR("这是一条error信息");
	Thread t1([&]()
			  {
				  LOG_DEBUG("这是一条debug信息");
			  });
	t1.start();
	LOG_FATAL("这是一条fatal信息");
}

TEST(LoggingTest, StreamAPI)
{
	LOGS_TRACE << "这是一条trace信息\n";
	LOGS_DEBUG << "这是一条debug信息\n";
	LOGS_INFO << "这是一条info信息\n";
	LOGS_WARN << "这是一条warn信息\n";
	LOGS_ERROR << "这是一条error信息\n";
	Thread t1([&]()
			  {
				  LOGS_DEBUG << "这是一条debug信息\n";
			  });
	t1.start();
	LOGS_FATAL << "这是一条流式fatal信息\n";
}

// change option LOG_WRITER_TYPE to 1（SINGLE_FILE）
TEST(LoggingTest, SingleFile)
{
	LOG_TRACE("这是一条trace信息");
	LOG_DEBUG("这是一条debug信息");
	LOG_INFO("这是一条info信息");
	LOG_WARN("这是一条warn信息");
	LOG_ERROR("这是一条error信息");
	LOG_FATAL("这是一条fatal信息");
	LOGS_TRACE << "这是一条trance信息 stream\n";
	LOGS_DEBUG << "这是一条debug信息 stream\n";
	LOGS_INFO << "这是一条info信息 stream\n";
	LOGS_WARN << "这是一条warn信息 stream\n";
	LOGS_ERROR << "这是一条error信息 stream\n";
	LOGS_FATAL << "这是一条流式fatal信息 stream\n";
	::sleepS(2);
}

// before execute:
// change LOG_WRITER_TYPE to 2（DAILY_FILE）
// and change the constant "SecondsPerDay(in LogWriter.cpp)" for short intervals
TEST(LoggingTest, DailyFile)
{
	LOG_TRACE("这是一条trace信息");
	LOG_DEBUG("这是一条debug信息");
	LOG_INFO("这是一条info信息");
	LOG_WARN("这是一条warn信息");
	LOG_ERROR("这是一条error信息");
	LOG_FATAL("这是一条fatal信息");
	LOGS_TRACE << "这是一条trance信息 stream\n";
	LOGS_DEBUG << "这是一条debug信息 stream\n";
	LOGS_INFO << "这是一条info信息 stream\n";
	LOGS_WARN << "这是一条warn信息 stream\n";
	LOGS_ERROR << "这是一条error信息 stream\n";
	LOGS_FATAL << "这是一条fatal信息 stream\n";
	::sleepS(2);
}


// before execute:
// change LOG_WRITER_TYPE to 3（DLOG_FILE_ROLLING_SIZE）
// and change constant "LogFileRollingSize(in LogWriter.cpp)" as small as possible
TEST(LoggingTest, RollingFile)
{
	LOGS_DEBUG << "这是一条足够长的信息这是一条足够长的信息这是一条足够长的信息这是一条足够长的信息这是一条足够长的信息这是一条足够长的信息"
				  "这是一条足够长的信息这是一条足够长的信息这是一条足够长的信息这是一条足够长的信息这是一条足够长的信息这是一条足够长的信息"
				  "这是一条足够长的信息这是一条足够长的信息这是一条足够长的信息这是一条足够长的信息这是一条足够长的信息这是一条足够长的信息";
	::sleepS(2);
}

int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}