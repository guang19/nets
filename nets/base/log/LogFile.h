//
// Created by guang19 on 2022/6/10.
//

#ifndef NETS_BASE_LOG_FILE_H
#define NETS_BASE_LOG_FILE_H

#include <ctime>
#include <memory>

#include "nets/base/Noncopyable.h"

namespace nets::base
{

	namespace
	{
		constexpr ::size_t FileIoBufferSize = 1024 * 256;
	}

	class LogFile : Noncopyable
	{
	public:
		using SizeType = ::size_t;
		using TimeType = ::time_t;
		using CharArrayPtr = ::std::unique_ptr<char[]>;

	public:
		explicit LogFile(const char* file);
		~LogFile();

	public:
		void append(const char* data, SizeType len);
		void flush();

		void renameByNowTime(TimeType now);

		void mkdirR(const char* multiLevelDir);

		inline SizeType size() const
		{
			return bytes_;
		}

		inline TimeType lastRollTime() const
		{
			return lastRollTime_;
		}

	private:
		void getFileInfo(uint64_t* fileSize, ::time_t* createTime);

	private:
		FILE* fp_ {nullptr};
		CharArrayPtr dir_ {nullptr};
		CharArrayPtr file_ {nullptr};
		SizeType bytes_ {0};
		TimeType lastRollTime_ {0};
		char buffer_[FileIoBufferSize] {0};
	};

} // namespace nets::base

#endif // NETS_BASE_LOG_FILE_H
