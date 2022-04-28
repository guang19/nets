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

class Lg
{
	public:
		Lg() : thread_()
		{
			n = new int(5);
		}

		~Lg()
		{
			if (running_)
			{
				stop();
			}
		}

		void stop()
		{
			running_ = false;
			delete n;
			n = nullptr;
		}

		void start()
		{
			running_ = true;
			thread_ = ::std::thread(&Lg::async, this);
			if (thread_.joinable())
			{
				thread_.detach();
			}
		}

		void async()
		{
			while (running_)
			{
				::std::this_thread::sleep_for(::std::chrono::milliseconds(2000));
				*n = *n + 1;
				printf("==%d==\n", *n);
			}
		}

	private:
		::std::thread thread_;
		::std::atomic<bool> running_ { false };
		int* n { nullptr };
};

#include <libgen.h>
#include <string.h>

void mkdirR(char* dir)
{
	char
}

TEST(LoggingTest, SingleFile)
{
//	LOG_TRACE("这是一条trace信息");
	const char file[] = "/tmp/nets/nets.log";
	FILE* fp = ::std::fopen(file, "a");
	const char* dir = dirname(const_cast<char*>(file));
	::std::cout << "目录为：" << dir << '\n';
	const char* basename_ = basename(const_cast<char*>(file));
	::std::cout << "basename:" << basename_ << '\n';
	::std::cout << file;
	if (fp == nullptr)
	{
		::std::cout << "创建或打开失败\n";

	}
	else
	{
		::std::cout << "创建或打开成功\n";
	}
}

int main(int argc, char** argv)
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}