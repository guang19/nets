//
// Created by YangGuang on 2022/4/17.
//

#include "base/log/LogWriter.h"

#include <cstring>
#include <ctime>
#include <libgen.h>
#include <sys/stat.h>
#include <unistd.h>
#include <utility>

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
			constexpr uint32_t FileBufferSize = 1024 << 3;
			constexpr uint32_t FilenameLen = 256;
		}

		LogFile::LogFile(const char* file)
		{
			uint32_t filenameLen = strlen(file);
			if (filenameLen > FilenameLen)
			{

				::std::fprintf(::stderr, "Error:log file name length more than %d\n", FilenameLen);
				exit(1);
			}
			char tmpFile[FilenameLen] = { 0 };
			::std::memset(tmpFile, 0, FilenameLen);
			::std::memcpy(tmpFile, file, filenameLen);
			dir_ = ::dirname(tmpFile);
			filename_ = ::basename(tmpFile);
			// log file has parent directory
			if (::std::strcmp(dir_, ".") != 0)
			{
				// create parent directory of log file
				mkdirR(dir_);
			}
			if ((fp_ = ::std::fopen(file, "a")) == nullptr)
			{
				::std::fprintf(::stderr, "Error:log file name length more than %d\n", FilenameLen);
				exit(1);
			}
			getFileInfo(&bytes_, &createTime_);
			buffer_ = new char[FileBufferSize];
			::std::memset(buffer_, 0, FileBufferSize);
			::setbuffer(fp_, buffer_, FileBufferSize);
		}

		LogFile::~LogFile()
		{
			if (fp_ != nullptr)
			{
				::std::fclose(fp_);
				fp_ = nullptr;
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
				// thread unsafe
				uint64_t n = ::fwrite_unlocked(data + writtenBytes, remain, 1, fp_);
				if (n != remain)
				{
					int32_t err = ferror(fp_);
					if (err)
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

		void LogFile::renameByTime()
		{
			if (fp_ != nullptr)
			{
				::std::fclose(fp_);
				fp_ = nullptr;
			}
			struct tm tmS {};
			::localtime_r(&createTime_, &tmS);
			char newFilename[23] = { 0 };
			::std::strftime(newFilename, 20, "%Y-%m-%d_%H:%M:%S", &tmS);
			::std::memcpy(newFilename + 19, ".log", 4);
			if (::std::strcmp(dir_, ".") != 0)
			{
				char tmpFile1[FilenameLen] = { 0 };
				char tmpFile2[FilenameLen] = { 0 };
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
			getFileInfo(&bytes_, &createTime_);
		}

		void LogFile::mkdirR(const char *multiLevelDir)
		{
			if (access(multiLevelDir, F_OK) == 0)
			{
				return;
			}
			uint32_t len = strlen(multiLevelDir);
			char dir1[256] = { 0 };
			char* dirptr1 = dir1;
			char dir2[256] = { 0 };
			::std::memset(dir1, 0, 256);
			::std::memset(dir2, 0, 256);
			::std::memcpy(dir1, multiLevelDir, len);
			char* spStr;
			while ((spStr = strsep(&dirptr1, "/")) != nullptr)
			{
				if (strlen(spStr) > 0)
				{
					::std::strcat(dir2, "/");
					::std::strcat(dir2, spStr);
					if (access(dir2, F_OK) != 0)
					{
						if (::mkdir(dir2, 0644) != 0)
						{
							::std::fprintf(::stderr, "Error: create parent directory of log file\n");
							exit(1);
						}
					}
				}
			}
		}

		void LogFile::getFileInfo(uint64_t* fileSize, ::std::time_t* createTime)
		{
			struct stat fileInfo {};
			if (!::fstat(fileno(fp_), &fileInfo))
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
			constexpr uint32_t LogBufferSize = 2 * 1024 * 1024;
			// Log buffer flush interval,unit：seconds
			constexpr uint32_t LogBufferFlushInterval= 1;
		}

		AsyncFileLogWriter::AsyncFileLogWriter() : logFile_(new LogFile(LOG_FILE)),
			running_(false), cacheBuffer_(new LogBuffer(LogBufferSize)),
			backupCacheBuffer_(new LogBuffer(LogBufferSize)), buffers_(),
			asyncThread_(&AsyncFileLogWriter::asyncWrite, this), mtx_(), cv_()
		{
			buffers_.reserve(LOG_FILE_ROLLING_SIZE >> 1);
		}

		AsyncFileLogWriter::~AsyncFileLogWriter()
		{
			if (running_)
			{
				stop();
			}
		}

		void AsyncFileLogWriter::start()
		{
			if (running_)
			{
				return;
			}
			running_ = true;
			if (asyncThread_.joinable())
			{
				asyncThread_.detach();
			}
		}

		void AsyncFileLogWriter::stop()
		{
			running_ = false;
			cv_.notify_one();
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
					cacheBuffer_.reset(new LogBuffer(LogBufferSize));
				}
				cacheBuffer_->append(data, len);
				cv_.notify_one();
			}
		}

		void AsyncFileLogWriter::asyncWrite()
		{
			BufferVectorType tmpBuffers;
			tmpBuffers.reserve(LOG_FILE_ROLLING_SIZE >> 1);
			BufferPtr tmpBuffer1(new LogBuffer(LogBufferSize));
			BufferPtr tmpBuffer2(new LogBuffer(LogBufferSize));
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
				for (const BufferPtr& buf : tmpBuffers)
				{
					persist(buf->getBuffer(), buf->length());
				}
				logFile_->flush();
				tmpBuffers.clear();
				tmpBuffer1.reset(new LogBuffer(LogBufferSize));
				if (tmpBuffer2 == nullptr)
				{
					tmpBuffer2.reset(new LogBuffer(LogBufferSize));
				}
			}
			logFile_->flush();
		}

		void AsyncSingleFileLogWriter::persist(const char* data, uint32_t len)
		{
			logFile_->append(data, len);
		}

		namespace
		{
			::std::time_t DaySeconds = 60 * 60 * 24;
		}

		void AsyncDailyFileLogWriter::persist(const char* data, uint32_t len)
		{
			::std::time_t nowTime = 0;
			::std::time(&nowTime);
			if (nowTime - logFile_->createTime() >= DaySeconds)
			{
				logFile_->renameByTime();
			}
			logFile_->append(data, len);
		}

		namespace
		{
			constexpr uint64_t LogFileRollingSize = LOG_FILE_ROLLING_SIZE * 1024 * 1024;
		}

		void AsyncRollingFileLogWriter::persist(const char* data, uint32_t len)
		{
			if (logFile_->size() >= LogFileRollingSize)
			{
				logFile_->renameByTime();
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
