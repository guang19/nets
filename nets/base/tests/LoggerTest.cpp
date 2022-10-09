//
// Created by guang19
//

#include <gtest/gtest.h>

#include <thread>

#include "nets/base/log/Logger.h"

using namespace nets;

TEST(LoggerTest, BasicUse)
{
    NETS_SYSTEM_LOG_TRACE << "这是一条trace信息";
    NETS_SYSTEM_LOG_DEBUG << "这是一条debug信息";
    NETS_SYSTEM_LOG_INFO << "这是一条info信息";
    NETS_SYSTEM_LOG_WARN << "这是一条warn信息";
    NETS_SYSTEM_LOG_ERROR << "这是一条error信息";
    ::std::thread t1(
        [&]()
        {
            NETS_SYSTEM_LOG_DEBUG << "这是一条debug信息";
        });
    t1.join();
    NETS_SYSTEM_LOG_FATAL << "这是一条流式fatal信息";
    ::std::this_thread::sleep_for(::std::chrono::milliseconds(500));
}

// change option LOG_WRITER_TYPE to 1（SINGLE_FILE）
TEST(LoggerTest, SingleFile)
{
    NETS_SYSTEM_LOG_TRACE << "这是一条trance信息 stream";
    NETS_SYSTEM_LOG_DEBUG << "这是一条debug信息 stream";
    NETS_SYSTEM_LOG_INFO << "这是一条info信息 stream";
    NETS_SYSTEM_LOG_WARN << "这是一条warn信息 stream";
    NETS_SYSTEM_LOG_ERROR << "这是一条error信息 stream";
    NETS_SYSTEM_LOG_FATAL << "这是一条流式fatal信息 stream";
}

// before execute:
// change LOG_WRITER_TYPE to 2（DAILY_FILE）
// for testing,  you better change the constant "SecondsPerDay(in LogSynchronizer.cpp)" for short intervals
TEST(LoggerTest, DailyFile)
{
    NETS_SYSTEM_LOG_TRACE << "这是一条trance信息 stream";
    NETS_SYSTEM_LOG_DEBUG << "这是一条debug信息 stream";
    NETS_SYSTEM_LOG_INFO << "这是一条info信息 stream";
    NETS_SYSTEM_LOG_WARN << "这是一条warn信息 stream";
    NETS_SYSTEM_LOG_ERROR << "这是一条error信息 stream";
    NETS_SYSTEM_LOG_FATAL << "这是一条fatal信息 stream";
}

// before execute:
// change LOG_WRITER_TYPE to 3（DLOG_FILE_ROLLING_SIZE）
// for testing,  you better change constant "LogFileRollingSize(in LogSynchronizer.cpp)" as small as possible
TEST(LoggerTest, RollingFile)
{
    NETS_SYSTEM_LOG_DEBUG << "这是一条足够长的信息这是一条足够长的信息这是一条足够长的信息这是一条足够长的信息这是一条足够长的信息这是"
        "一条足够长的信息一条足够长的信息一条足够长的信息";
}


int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}