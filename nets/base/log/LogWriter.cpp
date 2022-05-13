//
// Created by YangGuang on 2022/4/17.
//

#include "nets/base/log/LogWriter.h"

#include <cassert>
#include <cstring>
#include <libgen.h>
#include <sys/stat.h>
#include <unistd.h>

#include "nets/base/CommonMacro.h"

namespace nets::base
{
	namespace
	{
		constexpr ::size_t FileIOBufferSize = 1024 << 3;
		constexpr uint32_t MaxFilePathLen = 256;
	} // namespace

	LogFile::LogFile(const char* file)
	{
		::uint32_t filePathLen = ::strlen(file);
		if (filePathLen > MaxFilePathLen)
		{

			::fprintf(stderr, "Error:log file name length more than %d\n", MaxFilePathLen);
			::exit(1);
		}
		file_ = ::std::make_unique<char[]>(filePathLen);
		MEMZERO(file_.get(), filePathLen);
		::strncat(file_.get(), file, filePathLen);
		// <dirname> will modifies input str, so reallocate an array
		char tmpFile[MaxFilePathLen] = {0};
		::strncat(tmpFile, file, filePathLen);
		const char* dir = ::dirname(tmpFile);
		::uint32_t dirLen = ::strlen(dir);
		dir_ = ::std::make_unique<char[]>(dirLen);
		MEMZERO(dir_.get(), dirLen);
		::strncat(dir_.get(), dir, dirLen);
		// log file has parent directory
		if (::strcmp(dir, ".") != 0)
		{
			// create parent directory of log file
			mkdirR(dir);
		}
		if ((fp_ = ::fopen(file, "a")) == nullptr)
		{
			::fprintf(stderr, "Error:failed to open log file\n");
			::exit(1);
		}
		getFileInfo(&bytes_, &lastRollTime_);
		buffer_ = ::std::make_unique<char[]>(FileIOBufferSize);
		MEMZERO(buffer_.get(), FileIOBufferSize);
		::setbuffer(fp_, buffer_.get(), FileIOBufferSize);
	}

	LogFile::~LogFile()
	{
		if (fp_ != nullptr)
		{
			::fclose(fp_);
			fp_ = nullptr;
		}
	}

	void LogFile::append(const char* data, SizeType len)
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
					::fprintf(stderr, "Error:log file append error ""\"%s\"""\n", ::strerror(err));
					break;
				}
			}
			writtenBytes += n;
		}
		bytes_ += writtenBytes;
	}

	void LogFile::flush()
	{
		::fflush(fp_);
	}

	void LogFile::renameByNowTime(TimeType now)
	{
		if (fp_ != nullptr)
		{
			::fclose(fp_);
			fp_ = nullptr;
		}
		struct tm tmS {};
		::localtime_r(&now, &tmS);
		char newFilename[26] = {0};
		MEMZERO(newFilename, 26);
		::strftime(newFilename, 26, "%Y-%m-%d_%H-%M-%S", &tmS);
		::strcat(newFilename, ".log");
		if (::strcmp(dir_.get(), ".") != 0)
		{
			char tmpFile1[MaxFilePathLen] = {0};
			::strcat(tmpFile1, dir_.get());
			::strcat(tmpFile1, "/");
			::strcat(tmpFile1, newFilename);
			::rename(file_.get(), tmpFile1);
			fp_ = ::fopen(file_.get(), "a");
		}
		else
		{
			::rename(file_.get(), newFilename);
			fp_ = ::fopen(file_.get(), "a");
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
		uint32_t len = ::strlen(multiLevelDir);
		char dir1[MaxFilePathLen] = {0};
		char* dirptr = dir1;
		char dir2[MaxFilePathLen] = {0};
		MEMZERO(dir1, MaxFilePathLen);
		MEMZERO(dir2, MaxFilePathLen);
		::strncat(dir1, multiLevelDir, len);
		char* spStr = nullptr;
		while ((spStr = strsep(&dirptr, "/")) != nullptr)
		{
			if (::strlen(spStr) > 0)
			{
				::strcat(dir2, "/");
				::strcat(dir2, spStr);
				if (access(dir2, F_OK) != 0)
				{
					if (::mkdir(dir2, 0775) != 0)
					{
						::fprintf(stderr, "Error: failed to create parent directory of log file\n");
						::exit(1);
					}
				}
			}
		}
	}

	void LogFile::getFileInfo(SizeType* fileSize, TimeType* createTime)
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

	INIT_SINGLETON(StdoutPersistentWriter);
	INIT_SINGLETON(SingleLogFilePersistentWriter);
	INIT_SINGLETON(DailyLogFilePersistentWriter);
	INIT_SINGLETON(RollingLogFilePersistentWriter);

	void StdoutPersistentWriter::persist(const char* data, SizeType len, TimeType persistTime)
	{
		UNUSED(persistTime);
		::fwrite(data, 1, len, stdout);
	}

	void StdoutPersistentWriter::flush()
	{
		::fflush(stdout);
	}

	SingleLogFilePersistentWriter::SingleLogFilePersistentWriter() : logFile_(::std::make_unique<FileType>(LOG_FILE)) {}

	void SingleLogFilePersistentWriter::persist(const char* data, SizeType len, TimeType persistTime)
	{
		UNUSED(persistTime);
		logFile_->append(data, len);
	}

	void SingleLogFilePersistentWriter::flush()
	{
		logFile_->flush();
	}

	DailyLogFilePersistentWriter::DailyLogFilePersistentWriter() : logFile_(::std::make_unique<FileType>(LOG_FILE)) {}

	namespace
	{
		// if you need test DAILY_FILE LogWriter, you need to adjust this
		// constant for short intervals, not for the whole day
		 constexpr ::time_t SecondsPerDay = 60 * 60 * 24;
		 // Set SecondsPerDay to 30, then you can watch if the log file is roll back after 30s
		 // constexpr ::time_t SecondsPerDay = 30;
	} // namespace

	void DailyLogFilePersistentWriter::persist(const char* data, SizeType len, TimeType persistTime)
	{
		if (persistTime - logFile_->lastRollTime() >= SecondsPerDay)
		{
			logFile_->renameByNowTime(persistTime);
		}
		logFile_->append(data, len);
	}

	void DailyLogFilePersistentWriter::flush()
	{
		logFile_->flush();
	}

	RollingLogFilePersistentWriter::RollingLogFilePersistentWriter() : logFile_(::std::make_unique<FileType>(LOG_FILE)) {}

	namespace
	{
		// if you need test ROLLING_FILE LogWriter, you need to adjust this
		// constant  as small as possible
		constexpr ::size_t LogFileRollingSize = LOG_FILE_ROLLING_SIZE * 1024 * 1024;
		// Set LogFileRollingSize to 200 Bytes, then you will see soon if the log file is roll back
		// constexpr ::size_t LogFileRollingSize = 200;
	} // namespace

	void RollingLogFilePersistentWriter::persist(const char* data, SizeType len, TimeType persistTime)
	{
		if (logFile_->size() + len > LogFileRollingSize)
		{
			logFile_->renameByNowTime(persistTime);
		}
		logFile_->append(data, len);
	}

	void RollingLogFilePersistentWriter::flush()
	{
		logFile_->flush();
	}

	::std::shared_ptr<IPersistentWriter> PersistentWriterFactory::getPersistentWriter()
	{
		switch (LOG_WRITER_TYPE)
		{
			case LogWriterType::STDOUT:
				return StdoutPersistentWriter::getInstance();
			case LogWriterType::SINGLE_FILE:
				return SingleLogFilePersistentWriter::getInstance();
			case LogWriterType::DAILY_FILE:
				return DailyLogFilePersistentWriter::getInstance();
			case LogWriterType::ROLLING_FILE:
				return RollingLogFilePersistentWriter::getInstance();
			default:
				return StdoutPersistentWriter::getInstance();
		}
	}

	INIT_SINGLETON(AsyncLogWriter);

	namespace
	{
		// Log buffer cache 2M
		constexpr ::size_t MaxLogBufferSize =  1024 * 1024 << 1;
		// Log buffer flush interval,unit：milliseconds
		constexpr ::time_t LogBufferFlushInterval = 1000;
	} // namespace

	AsyncLogWriter::AsyncLogWriter()
		: running_(false), cacheBuffer_(::std::make_unique<BufferType>(MaxLogBufferSize)),
		  backupCacheBuffer_(::std::make_unique<BufferType>(MaxLogBufferSize)),
		  persistentWriter_(PersistentWriterFactory::getPersistentWriter()), mutex_(), cv_(),
		  buffers_(::std::make_unique<BufferVectorType>()), writerTaskQueue_(::std::make_unique<BlockingQueueType>())
	{
		buffers_->reserve(LOG_FILE_ROLLING_SIZE >> 1);
	}

	AsyncLogWriter::~AsyncLogWriter()
	{
		if (running_)
		{
			running_ = false;
			cv_.notify_one();
			if (writerTaskProducer.joinable())
			{
				writerTaskProducer.join();
			}
			if (writerTaskConsumer.joinable())
			{
				writerTaskConsumer.join();
			}
		}
	}

	void AsyncLogWriter::write(const char* data, SizeType len)
	{
		LockGuardType lock(mutex_);
		if (cacheBuffer_->writeableBytes() > len)
		{
			cacheBuffer_->appendStr(data, len);
		}
		else
		{
			buffers_->push_back(::std::move(cacheBuffer_));
			if (backupCacheBuffer_ != nullptr)
			{
				cacheBuffer_ = ::std::move(backupCacheBuffer_);
			}
			else
			{
				cacheBuffer_ = std::make_unique<LogBuffer>(MaxLogBufferSize);
			}
			cacheBuffer_->appendStr(data, len);
			cv_.notify_one();
		}
	}

	void AsyncLogWriter::start()
	{
		if (running_)
		{
			return;
		}
		running_ = true;
		writerTaskProducer = ::std::thread(&AsyncLogWriter::asyncWrite, this);
		writerTaskConsumer = ::std::thread(&AsyncLogWriter::asyncPersist, this);
	}

	void AsyncLogWriter::asyncWrite()
	{
		auto buffers = ::std::make_unique<BufferVectorType>();
		buffers->reserve(LOG_FILE_ROLLING_SIZE >> 1);
		BufferPtr tmpBuffer1 = ::std::make_unique<LogBuffer>(MaxLogBufferSize);
		BufferPtr tmpBuffer2 = ::std::make_unique<LogBuffer>(MaxLogBufferSize);
		TimeType currentTime = 0;
		while (running_)
		{
			{
				UniqueLockType lock(mutex_);
				// first determine whether the buffer vector is empty, and then determine whether cache buffer has data
				cv_.wait_for(lock, ::std::chrono::milliseconds(LogBufferFlushInterval),
							 [this]() -> bool
							 {
								 return !buffers_->empty();
							 });
				if (buffers_->empty() && cacheBuffer_->readableBytes() <= 0)
				{
					lock.unlock();
					continue;
				}
				buffers_->push_back(::std::move(cacheBuffer_));
				buffers->swap(*buffers_);
				cacheBuffer_ = ::std::move(tmpBuffer1);
				if (backupCacheBuffer_ == nullptr)
				{
					backupCacheBuffer_ = ::std::move(tmpBuffer2);
				}
			}
			::time(&currentTime);
			auto writerTask = [this, tmpBuffers = buffers.release(), currentTime]() mutable
			{
				for (auto it = tmpBuffers->begin(), end = tmpBuffers->end(); it != end; ++it)
				{
					auto logBuffer = it->get();
					persistentWriter_->persist(logBuffer->buffer(), logBuffer->len(), currentTime);
				}
				persistentWriter_->flush();
			};
			// if not running, the writer task that is not added to the queue will be discarded
			writerTaskQueue_->put(::std::move(writerTask));
			buffers = ::std::make_unique<BufferVectorType>();
			tmpBuffer1 = std::make_unique<LogBuffer>(MaxLogBufferSize);
			if (tmpBuffer2 == nullptr)
			{
				tmpBuffer2 = std::make_unique<LogBuffer>(MaxLogBufferSize);
			}
		}
		// last check
		assert(!running_);
		if (!buffers_->empty() || cacheBuffer_->readableBytes() > 0)
		{
			buffers_->push_back(::std::move(cacheBuffer_));
			::time(&currentTime);
			auto writerTask = [this, tmpBuffers = buffers_.release(), currentTime]() mutable
			{
				for (auto it = tmpBuffers->begin(), end = tmpBuffers->end(); it != end; ++it)
				{
					auto logBuffer = it->get();
					persistentWriter_->persist(logBuffer->buffer(), logBuffer->len(), currentTime);
				}
				persistentWriter_->flush();
			};
			writerTaskQueue_->put(::std::move(writerTask));
		}
	}

	void AsyncLogWriter::asyncPersist()
	{
		WriterTaskType writerTask = nullptr;
		while (running_)
		{
			if (writerTaskQueue_->take(writerTask, LogBufferFlushInterval))
			{
				writerTask();
				writerTask = nullptr;
			}
		}
		// last check
		assert(!running_);
		if (writerTaskQueue_->take(writerTask, LogBufferFlushInterval))
		{
			writerTask();
			writerTask = nullptr;
		}
	}
} // namespace nets::base