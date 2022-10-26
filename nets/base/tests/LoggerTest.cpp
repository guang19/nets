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

// @brief Test for Logger

#include <gtest/gtest.h>

#include <thread>

#include "nets/base/log/Logger.h"

using namespace nets;

TEST(LoggerTest, BasicUse)
{
    ASSERT_EQ(kNetsRootLogger->getLevel(), LogLevel::DEBUG);
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
    kNetsRootLogger->setLogAppender(FileLogAppender::createFileLogAppender("/tmp/nets/nets.log", LogFileType::DAILY_FILE));
    NETS_SYSTEM_LOG_TRACE << "这是一条trance信息 stream，验证DailyFile";
    NETS_SYSTEM_LOG_DEBUG << "这是一条debug信息 stream，验证DailyFile";
    NETS_SYSTEM_LOG_INFO << "这是一条info信息 stream，验证DailyFile";
    NETS_SYSTEM_LOG_WARN << "这是一条warn信息 stream，验证DailyFile";
    NETS_SYSTEM_LOG_ERROR << "这是一条error信息 stream，验证DailyFile";
    NETS_SYSTEM_LOG_FATAL << "这是一条fatal信息 stream，验证DailyFile";
}

TEST(LoggerTest, RollingFile)
{
    kNetsRootLogger->setLogAppender(FileLogAppender::createFileLogAppender("/tmp/nets/nets.log", LogFileType::ROLLING_FILE));
    kNetsRootLogger->setLogFileRollingSize(1);
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