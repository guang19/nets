//
// Created by YangGuang on 2022/4/17.
//

#include "base/log/LogWriter.h"

#include <cstring>
#include <ctime>
#include <utility>

namespace nets
{
	namespace base
	{
		INIT_SINGLETON(StdoutLogWriter)
		INIT_SINGLETON(AsyncSingleFileLogWriter)
		INIT_SINGLETON(AsyncDailyFileLogWriter)
		INIT_SINGLETON(AsyncRollingFileLogWriter)
		INIT_SINGLETON(LogWriterFactory)

		void StdoutLogWriter::write(const char* data, uint32_t len)
		{
			::std::fwrite(data,1, len, ::stdout);
			::std::fflush(::stdout);
		}

		namespace
		{
			constexpr uint32_t FileBufferSize = 1024 << 3;
		}

		LogFile::LogFile(const char* file) : dir_(file), filename_(), bytes_(0)
		{
			::std::string::size_type idx = dir_.find_last_of('/');
			if (idx != ::std::string::npos)
			{
				dir_.erase(dir_.begin() + idx + 1, dir_.end());
				filename_.append(file + idx  + 1);
			}
			else
			{
				dir_.clear();
				filename_.append(file);
			}
			fp_ = ::std::fopen(file, "a");
			getFileInfo(nullptr, &createTime_, nullptr);
			buffer_ = new char[FileBufferSize];
			::std::memset(buffer_, 0, FileBufferSize);
			::setbuffer(fp_, buffer_, FileBufferSize);
		}

		LogFile::~LogFile()
		{
			if (fp_ != NULL)
			{
				::std::fclose(fp_);
				fp_ = NULL;
			}
			if (buffer_ != nullptr)
			{
				delete[] buffer_;
				buffer_ = nullptr;
			}
		}

		void LogFile::append(const char* data, uint32_t len)
		{
			uint32_t writtenBytes = 0;
			while (writtenBytes < len)
			{
				uint32_t remain = len - writtenBytes;
				// thread unsafe
				uint32_t n = ::fwrite_unlocked(data + writtenBytes, remain, 1, fp_);
				if (n != remain)
				{
					int32_t err = ferror(fp_);
					if (err)
					{
						::std::fprintf(::stderr, "LogFile append error:%s", strerror(err));
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

		void LogFile::renameByNowTime()
		{
			if (fp_ != NULL)
			{
				::std::fclose(fp_);
				fp_ = NULL;
			}
			struct tm tmS {};
			::localtime_r(&createTime_, &tmS);
			char newFilename[25] = { 0 };
			::std::strftime(newFilename, 20, "%Y-%m-%d_%H:%M:%S", &tmS);
			::std::memcpy(newFilename + strlen(newFilename), ".log", 4);
			if (dir_.empty())
			{
				const char* fileCStr = filename_.c_str();
				::std::rename(fileCStr, newFilename);
				fp_ = ::std::fopen(fileCStr, "a");
			}
			else
			{
				::std::string dir1(dir_);
				::std::string dir2(dir_);
				const char* fileCStr = dir1.append(filename_).c_str();
				::std::rename(fileCStr, dir2.append(newFilename).c_str());
				fp_ = ::std::fopen(fileCStr, "a");
			}
			getFileInfo(nullptr, &createTime_, nullptr);
			bytes_ = 0;
		}

		void LogFile::getFileInfo(uint64_t* fileSize, ::std::time_t* createTime, ::std::time_t* modifyTime)
		{
			FileInfo fileInfo {};
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
			if (modifyTime != nullptr)
			{
				*modifyTime = fileInfo.st_mtime;
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
				logFile_->renameByNowTime();
			}
			logFile_->append(data, len);
		}

		void AsyncRollingFileLogWriter::persist(const char* data, uint32_t len)
		{
			if (logFile_->size() >= LOG_FILE_ROLLING_SIZE)
			{
				logFile_->renameByNowTime();
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
				{
					AsyncSingleFileLogWriter* logWriter = AsyncSingleFileLogWriter::getInstance();
					logWriter->start();
					return logWriter;
				}
				case LogWriterType::DAILY_FILE:
				{
					AsyncDailyFileLogWriter* logWriter = AsyncDailyFileLogWriter::getInstance();
					logWriter->start();
					return logWriter;
				}
				case LogWriterType::ROLLING_FILE:
				{
					AsyncRollingFileLogWriter* logWriter = AsyncRollingFileLogWriter::getInstance();
					logWriter->start();
					return logWriter;
				}
				default:
					return StdoutLogWriter::getInstance();
			}
		}
	} // namespace base
} // namespace nets
