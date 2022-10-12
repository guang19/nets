//
// Created by guang19
//

#include <gtest/gtest.h>

#include <thread>

#include "nets/base/log/Logger.h"

using namespace nets;

TEST(LoggerTest, BasicUse)
{
    ASSERT_EQ(kRootLogger->getLevel(), LogLevel::DEBUG);
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
}

TEST(LoggerTest, SingleFile)
{
    LOGGER_MGR->getRootLogger()->setLogAppender(
        FileLogAppender::createFileLogAppender("/tmp/nets/nets.log", LogFileType::SINGLE_FILE));
    NETS_SYSTEM_LOG_TRACE << "这是一条trance信息 stream，验证SingleFile";
    NETS_SYSTEM_LOG_DEBUG << "这是一条debug信息 stream，验证SingleFile";
    NETS_SYSTEM_LOG_INFO << "这是一条info信息 stream，验证SingleFile";
    NETS_SYSTEM_LOG_WARN << "这是一条warn信息 stream，验证SingleFile";
    NETS_SYSTEM_LOG_ERROR << "这是一条error信息 stream，验证SingleFile";
    NETS_SYSTEM_LOG_FATAL << "这是一条流式fatal信息 stream，验证SingleFile";
}

// before execute:
// for testing,  you better change the constant "kSecondsPerDay(in LogAppender.h)" for short intervals
TEST(LoggerTest, DailyFile)
{
    kRootLogger->setLogAppender(FileLogAppender::createFileLogAppender("/tmp/nets/nets.log", LogFileType::DAILY_FILE));
    NETS_SYSTEM_LOG_TRACE << "这是一条trance信息 stream，验证DailyFile";
    NETS_SYSTEM_LOG_DEBUG << "这是一条debug信息 stream，验证DailyFile";
    NETS_SYSTEM_LOG_INFO << "这是一条info信息 stream，验证DailyFile";
    NETS_SYSTEM_LOG_WARN << "这是一条warn信息 stream，验证DailyFile";
    NETS_SYSTEM_LOG_ERROR << "这是一条error信息 stream，验证DailyFile";
    NETS_SYSTEM_LOG_FATAL << "这是一条fatal信息 stream，验证DailyFile";
}

TEST(LoggerTest, RollingFile)
{
    kRootLogger->setLogAppender(FileLogAppender::createFileLogAppender("/tmp/nets/nets.log", LogFileType::ROLLING_FILE));
    kRootLogger->setLogFileRollingSize(1);
    NETS_SYSTEM_LOG_DEBUG
        << "这是一条足够长的信息这是一条足够长的信息这是一条足够长的信息这是一条足够长的信息这是一条足够长的信息这是"
           "一条足够长的信息一条足够长的信息一条足够长的信息一条足够长的信息一条足够长的信息一条足够长的信息"
           "一条足够长的信息一条足够长的信息一条足够长的信息一条足够长的信息一条足够长的信息一条足够长的信息"
           "一条足够长的信息一条足够长的信息一条足够长的信息一条足够长的信息一条足够长的信息一条足够长的信息"
           "一条足够长的信息一条足够长的信息一条足够长的信息一条足够长的信息一条足够长的信息一条足够长的信息"
           "一条足够长的信息一条足够长的信息一条足够长的信息一条足够长的信息一条足够长的信息一条足够长的信息"
           "一条足够长的信息一条足够长的信息一条足够长的信息一条足够长的信息一条足够长的信息一条足够长的信息"
           "一条足够长的信息一条足够长的信息一条足够长的信息一条足够长的信息一条足够长的信息一条足够长的信息"
           "一条足够长的信息一条足够长的信息一条足够长的信息一条足够长的信息一条足够长的信息一条足够长的信息"
           "一条足够长的信息一条足够长的信息一条足够长的信息一条足够长的信息一条足够长的信息一条足够长的信息"
           "一条足够长的信息一条足够长的信息一条足够长的信息一条足够长的信息一条足够长的信息一条足够长的信息，验证RollingFil"
           "e";
    ::sleep(1);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}