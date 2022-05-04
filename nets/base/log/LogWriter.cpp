//
// Created by YangGuang on 2022/4/17.
//

#include "nets/base/log/LogWriter.h"

#include <cstring>
#include <ctime>
#include <libgen.h>
#include <sys/stat.h>
#include <unistd.h>
#include <utility>
#include "nets/base/CommonMacro.h"

namespace nets
{
	namespace base
	{
		INIT_SINGLETON(StdoutLogWriter);
		INIT_SINGLETON(AsyncSingleFileLogWriter);
		INIT_SINGLETON(AsyncDailyFileLogWriter);
		INIT_SINGLETON(AsyncRollingFileLogWriter);
		INIT_SINGLETON(LogWriterFactory);

		void StdoutLogWriter::write(const char* data, uint32_t len)
		{
			::std::fwrite(data,1, len, ::stdout);
			::std::fflush(::stdout);
		}

		namespace
		{
			constexpr uint32_t FileIOBufferSize = 1024 << 3;
			constexpr uint32_t MaxFilePathLen = 256;
		}

		LogFile::LogFile(const char* file)
		{
			uint32_t filePathLen = strlen(file);
			if (filePathLen > MaxFilePathLen)
			{

				::std::fprintf(::stderr, "Error:log file name length more than %d\n", MaxFilePathLen);
				abort();
			}
			char tmpFile[MaxFilePathLen] = { 0 };
			::std::memset(tmpFile, 0, MaxFilePathLen);
			::std::memcpy(tmpFile, file, filePathLen);
			// <dirname> will modify input str, so called <basename> before <dirname>
			const char* filename = ::basename(tmpFile);
			uint32_t filenameLen = ::std::strlen(filename);
			const char* dir = ::dirname(tmpFile);
			uint32_t dirLen = ::std::strlen(dir);
			dir_ = new char[dirLen + 1];
			filename_ = new char[filenameLen + 1];
			::std::memset(dir_, 0, dirLen + 1);
			::std::memcpy(dir_, dir, dirLen);
			::std::memset(filename_, 0, filenameLen + 1);
			::std::memcpy(filename_, filename, filenameLen);
			// log file has parent directory
			if (::std::strcmp(dir, ".") != 0)
			{
				// create parent directory of log file
				mkdirR(dir);
			}
			if ((fp_ = ::std::fopen(file, "a")) == nullptr)
			{
				::std::fprintf(::stderr, "Error:failed to open log file");
				abort();
			}
			getFileInfo(&bytes_, &lastRollTime_);
			buffer_ = new char[FileIOBufferSize];
			::std::memset(buffer_, 0, FileIOBufferSize);
			::setbuffer(fp_, buffer_, FileIOBufferSize);
		}

		LogFile::~LogFile()
		{
			if (fp_ != nullptr)
			{
				::std::fclose(fp_);
				fp_ = nullptr;
			}
			if (dir_ != nullptr)
			{
				delete[] dir_;
				dir_ = nullptr;
			}
			if (filename_ != nullptr)
			{
				delete[] filename_;
				filename_ = nullptr;
			}
			if (buffer_ != nullptr)
			{
				delete[] buffer_;
				buffer_ = nullptr;
			}
		}

		void LogFile::append(const char* data, uint32_t len)
		{
			uint64_t writtenBytes = 0;
			while (writtenBytes < len)
			{
				uint64_t remain = len - writtenBytes;
				// not thread-safe
				uint64_t n = ::fwrite_unlocked(data + writtenBytes, 1, remain, fp_);
				if (n != remain)
				{
					int32_t err = ferror(fp_);
					if (err != 0)
					{
						::std::fprintf(::stderr, "Error:log file append error ""\"""%s""\"""\n", ::strerror(err));
						break;
					}
				}
				writtenBytes += n;
			}
			bytes_ += writtenBytes;
		}

		void LogFile::flush()
		{
			::std::fflush(fp_);
		}

		void LogFile::renameByNowTime(::std::time_t now)
		{
			if (fp_ != nullptr)
			{
				::std::fclose(fp_);
				fp_ = nullptr;
			}
			struct tm tmS {};
			::localtime_r(&now, &tmS);
			char newFilename[24] = { 0 };
			::std::memset(newFilename, 0, 24);
			::std::strftime(newFilename, 20, "%Y-%m-%d_%H-%M-%S", &tmS);
			::std::memcpy(newFilename + 19, ".log", 4);
			if (::std::strcmp(dir_, ".") != 0)
			{
				char tmpFile1[MaxFilePathLen] = { 0 };
				char tmpFile2[MaxFilePathLen] = { 0 };
				::std::strcat(tmpFile1, dir_);
				::std::strcat(tmpFile1, "/");
				::std::strcat(tmpFile1, filename_);
				::std::strcat(tmpFile2, dir_);
				::std::strcat(tmpFile2, "/");
				::std::strcat(tmpFile2, newFilename);
				::std::rename(tmpFile1, tmpFile2);
				fp_ = ::std::fopen(tmpFile1, "a");
			}
			else
			{
				::std::rename(filename_, newFilename);
				fp_ = ::std::fopen(filename_, "a");
			}
			getFileInfo(&bytes_, nullptr);
			lastRollTime_ = now;
		}

		void LogFile::mkdirR(const char* multiLevelDir)
		{
			if (access(multiLevelDir, F_OK) == 0)
			{
				return;
			}
			uint32_t len = strlen(multiLevelDir);
			char dir1[MaxFilePathLen] = { 0 };
			char* dirptr = dir1;
			char dir2[MaxFilePathLen] = { 0 };
			::std::memset(dir1, 0, MaxFilePathLen);
			::std::memset(dir2, 0, MaxFilePathLen);
			::std::memcpy(dir1, multiLevelDir, len);
			char* spStr;
			while ((spStr = strsep(&dirptr, "/")) != nullptr)
			{
				if (strlen(spStr) > 0)
				{
					::std::strcat(dir2, "/");
					::std::strcat(dir2, spStr);
					if (access(dir2, F_OK) != 0)
					{
						if (::mkdir(dir2, 0775) != 0)
						{
							::std::fprintf(::stderr, "Error: failed to create parent directory of log file\n");
							abort();
						}
					}
				}
			}
		}

		void LogFile::getFileInfo(uint64_t* fileSize, ::std::time_t* createTime)
		{
			struct stat fileInfo {};
			if (::fstat(::fileno(fp_), &fileInfo) != 0)
			{
				return;
			}
			if (fileSize != nullptr)
			{
				*fileSize = fileInfo.st_size;
			}
			if (createTime != nullptr)
			{
				*createTime = fileInfo.st_ctime;
			}
		}

		namespace
		{
			// Log buffer cache 2M
			constexpr uint32_t MaxLogBufferSize = 2 * 1024 * 1024;
			// Log buffer flush interval,unit：milliseconds
			constexpr uint32_t LogBufferFlushInterval= 1000;
		}

		AsyncFileLogWriter::AsyncFileLogWriter() : logFile_(new LogFile(LOG_FILE)),
			running_(false), cacheBuffer_(new LogBuffer(MaxLogBufferSize)),
			backupCacheBuffer_(new LogBuffer(MaxLogBufferSize)), buffers_(), asyncThread_(),
			mtx_(), cv_()
		{
			buffers_.reserve(LOG_FILE_ROLLING_SIZE >> 1);
		}

		AsyncFileLogWriter::~AsyncFileLogWriter()
		{
			stop();
		}

		void AsyncFileLogWriter::start()
		{
			if (running_)
			{
				return;
			}
			running_ = true;
			asyncThread_ = ::std::thread(&AsyncFileLogWriter::asyncWrite, this);
		}

		void AsyncFileLogWriter::stop()
		{
			if (running_)
			{
				running_ = false;
				cv_.notify_one();
				if (asyncThread_.joinable())
				{
					asyncThread_.join();
				}
			}
		}

		void AsyncFileLogWriter::write(const char* data, uint32_t len)
		{
			UniqueLockType lock(mtx_);
			if (cacheBuffer_->available() > len)
			{
				cacheBuffer_->append(data, len);
			}
			else
			{
				buffers_.push_back(::std::move(cacheBuffer_));
				if (backupCacheBuffer_ != nullptr)
				{
					cacheBuffer_ = ::std::move(backupCacheBuffer_);
				}
				else
				{
					cacheBuffer_.reset(new LogBuffer(MaxLogBufferSize));
				}
				cacheBuffer_->append(data, len);
				cv_.notify_one();
			}
		}

		void AsyncFileLogWriter::asyncWrite()
		{
			BufferVectorType tmpBuffers;
			tmpBuffers.reserve(LOG_FILE_ROLLING_SIZE >> 1);
			BufferPtr tmpBuffer1(new LogBuffer(MaxLogBufferSize));
			BufferPtr tmpBuffer2(new LogBuffer(MaxLogBufferSize));
			::std::time_t currentTime = 0;
			while (running_)
			{
				{
					UniqueLockType lock(mtx_);
					cv_.wait_for(lock, ::std::chrono::milliseconds(LogBufferFlushInterval),
						[this]() -> bool
						{
							return !buffers_.empty();
						});
					buffers_.push_back(::std::move(cacheBuffer_));
					tmpBuffers.swap(buffers_);
					cacheBuffer_ = ::std::move(tmpBuffer1);
					if (backupCacheBuffer_ == nullptr)
					{
						backupCacheBuffer_ = ::std::move(tmpBuffer2);
					}
				}
				currentTime = ::std::time(nullptr);
				for (const BufferPtr& buf : tmpBuffers)
				{
					persist(buf->getBuffer(), buf->length(), currentTime);
				}
				logFile_->flush();
				tmpBuffers.clear();
				tmpBuffer1.reset(new LogBuffer(MaxLogBufferSize));
				if (tmpBuffer2 == nullptr)
				{
					tmpBuffer2.reset(new LogBuffer(MaxLogBufferSize));
				}
			}
			logFile_->flush();
		}

		void AsyncSingleFileLogWriter::persist(const char* data, uint32_t len, ::std::time_t persistTime)
		{
			UNUSED(persistTime);
			logFile_->append(data, len);
		}

		namespace
		{
			// if you need test DAILY_FILE LogWriter, you need to adjust this
			// constant for short intervals, not for the whole day
			constexpr ::std::time_t SecondsPerDay = 60 * 60 * 24;
			// Set SecondsPerDay to 30, then you can watch if the log file is roll back after 30s
			// constexpr ::std::time_t SecondsPerDay = 60;
		}

		void AsyncDailyFileLogWriter::persist(const char* data, uint32_t len, ::std::time_t persistTime)
		{
			if (persistTime - logFile_->getLastRollTime() >= SecondsPerDay)
			{
				logFile_->renameByNowTime(persistTime);
			}
			logFile_->append(data, len);
		}

		namespace
		{
			// if you need test ROLLING_FILE LogWriter, you need to adjust this
			// constant  as small as possible
			constexpr uint64_t LogFileRollingSize = LOG_FILE_ROLLING_SIZE * 1024 * 1024;
			// Set LogFileRollingSize to 500 Bytes, then you will see soon if the log file is roll back
//			 constexpr uint64_t LogFileRollingSize = 500;
		}

		void AsyncRollingFileLogWriter::persist(const char* data, uint32_t len, ::std::time_t persistTime)
		{
			if (logFile_->size() + len > LogFileRollingSize)
			{
				logFile_->renameByNowTime(persistTime);
			}
			logFile_->append(data, len);
		}

		ILogWriter* LogWriterFactory::getLogWriter() const
		{
			switch (LOG_WRITER_TYPE)
			{
				case LogWriterType::STDOUT:
					return StdoutLogWriter::getInstance();
				case LogWriterType::SINGLE_FILE:
					return AsyncSingleFileLogWriter::getInstance();
				case LogWriterType::DAILY_FILE:
					return AsyncDailyFileLogWriter::getInstance();
				case LogWriterType::ROLLING_FILE:
					return AsyncRollingFileLogWriter::getInstance();
				default:
					return StdoutLogWriter::getInstance();
			}
		}
	} // namespace base
} // namespace nets