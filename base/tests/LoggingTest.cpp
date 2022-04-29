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

// 将LOG_WRITER_TYPE选项改为1（SINGLE_FILE）
TEST(LoggingTest, SingleFile)
{
	LOG_TRACE("这是一条trace信息");
	LOG_DEBUG("这是一条debug信息");
	LOG_INFO("这是一条info信息");
	LOG_WARN("这是一条warn信息");
	LOG_ERROR("这是一条error信息");
	LOG_FATAL("这是一条fatal信息");
	LOGS_DEBUG << "这是一条debug信息 stream\n";
	LOGS_INFO << "这是一条info信息 stream\n";
	LOGS_WARN << "这是一条warn信息 stream\n";
	LOGS_ERROR << "这是一条error信息 stream\n";
	LOGS_FATAL << "这是一条fatal信息 stream\n";
	::std::this_thread::sleep_for(::std::chrono::milliseconds(2000));
}

#include <libgen.h>
#include <string.h>
// 将LOG_WRITER_TYPE选项改为2（DAILY_FILE）
TEST(LoggingTest, DailyFile)
{
	char tmpFile[256] = { 0 };
	::std::memset(tmpFile, 0, 256);
	::std::memcpy(tmpFile, "/tmp/nets/nets.log", 18);
	const char* filename = basename(tmpFile);
	uint32_t filenameLen = ::std::strlen(filename);
	const char* dir = ::dirname(tmpFile);
	uint32_t dirLen = ::std::strlen(dir);
	::std::cout << dirLen << '\t' << dir << '\t' << filenameLen << '\t' << filename;
}

int main(int argc, char** argv)
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}