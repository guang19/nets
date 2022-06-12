//
// Created by YangGuang on 2022/4/17.
//

#include "nets/base/log/LogWriter.h"

#include <cassert>

namespace nets::base
{
	namespace
	{
		// if you want to test DAILY_FILE LogWriter, you need to adjust this
		// constant for short intervals, not for the whole day
		constexpr ::time_t SecondsPerDay = 60 * 60 * 24;
		// Set SecondsPerDay to 30, then you can watch if the log file is roll back after 30s
		// constexpr ::time_t SecondsPerDay = 30;

		// if you want to test ROLLING_FILE LogWriter, you need to adjust this
		// constant  as small as possible
		constexpr ::size_t LogFileRollingSize = LOG_FILE_ROLLING_SIZE * 1024 * 1024;
		// Set LogFileRollingSize to 200 Bytes, then you will see soon if the log file is roll back
		// constexpr ::size_t LogFileRollingSize = 200;

		// Log buffer flush interval,unit：milliseconds
		constexpr ::time_t LogBufferFlushInterval = 1000;
	} // namespace

	INIT_SINGLETON(StdoutPersistentWriter);
	INIT_SINGLETON(SingleLogFilePersistentWriter);
	INIT_SINGLETON(DailyLogFilePersistentWriter);
	INIT_SINGLETON(RollingLogFilePersistentWriter);
	INIT_SINGLETON(AsyncLogWriter);

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
			case LogWriterType::Stdout:
				return StdoutPersistentWriter::getInstance();
			case LogWriterType::SingFile:
				return SingleLogFilePersistentWriter::getInstance();
			case LogWriterType::DailyFile:
				return DailyLogFilePersistentWriter::getInstance();
			case LogWriterType::RollingFile:
				return RollingLogFilePersistentWriter::getInstance();
			default:
				return StdoutPersistentWriter::getInstance();
		}
	}

	AsyncLogWriter::AsyncLogWriter()
		: running_(false), cacheBuffer_(::std::make_unique<BufferType>()),
		  backupCacheBuffer_(::std::make_unique<BufferType>()),
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
			cacheBuffer_->append(data, len);
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
				cacheBuffer_ = std::make_unique<BufferType>();
			}
			cacheBuffer_->append(data, len);
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
		auto tmpBuffer1 = ::std::make_unique<BufferType>();
		auto tmpBuffer2 = ::std::make_unique<BufferType>();
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
				if (buffers_->empty() && cacheBuffer_->len() <= 0)
				{
					lock.unlock();
					continue;
				}
				buffers_->push_back(::std::move(cacheBuffer_));
				buffers->swap(*buffers_);
				cacheBuffer_ = ::std::move(tmpBuffer1);
				if (nullptr == backupCacheBuffer_)
				{
					backupCacheBuffer_ = ::std::move(tmpBuffer2);
				}
			}
			::time(&currentTime);
			auto writerTask = [this, tmpBuffers = buffers.release(), currentTime]() mutable
			{
				for (auto& it: *tmpBuffers)
				{
					auto logBuffer = it.get();
					persistentWriter_->persist(logBuffer->carray(), logBuffer->len(), currentTime);
				}
				persistentWriter_->flush();
			};
			// if not running, the writer task that is not added to the queue will be discarded
			writerTaskQueue_->put(::std::move(writerTask));
			buffers = ::std::make_unique<BufferVectorType>();
			tmpBuffer1 = std::make_unique<BufferType>();
			if (nullptr == tmpBuffer2)
			{
				tmpBuffer2 = std::make_unique<BufferType>();
			}
		}
		// last check
		assert(!running_);
		if (!buffers_->empty() || (cacheBuffer_->len() > 0))
		{
			buffers_->push_back(::std::move(cacheBuffer_));
			::time(&currentTime);
			auto writerTask = [this, tmpBuffers = buffers_.release(), currentTime]() mutable
			{
				for (auto& it: *tmpBuffers)
				{
					auto logBuffer = it.get();
					persistentWriter_->persist(logBuffer->carray(), logBuffer->len(), currentTime);
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
			if (writerTaskQueue_->take(writerTask, LogBufferFlushInterval >> 1))
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