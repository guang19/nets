//
// Created by YangGuang on 2022/5/2.
//

#include "nets/base/concurrency/Mutex.h"

#include "nets/base/ThreadHelper.h"

namespace nets
{
	namespace base
	{
		Mutex::Mutex() : owner_(0)
		{
			pthread_mutex_init(&mutex_, nullptr);
		}

		Mutex::~Mutex()
		{
			pthread_mutex_destroy(&mutex_);
		}

		void Mutex::lock()
		{
			pthread_mutex_lock(&mutex_);
			owner_ = currentTid();
		}
		bool Mutex::tryLock()
		{
			if (pthread_mutex_trylock(&mutex_) == 0)
			{
				owner_ = currentTid();
				return true;
			}
			return false;
		}

		void Mutex::unlock()
		{
			owner_ = 0;
			pthread_mutex_unlock(&mutex_);
		}

		bool Mutex::isLockedByCurrentThread() const
		{
			return (owner_ == currentTid());
		}

		Mutex::OwnerGuard_::OwnerGuard_(Mutex &mutex) : mutex_(mutex)
		{
			mutex_.owner_ = 0;
		}

		Mutex::OwnerGuard_::~OwnerGuard_()
		{
			mutex_.owner_ = currentTid();
		}

		RecursiveMutex::RecursiveMutex() : owner_(0), count_(0)
		{
			pthread_mutexattr_t mutexattr {};
			pthread_mutexattr_settype(&mutexattr, PTHREAD_MUTEX_RECURSIVE);
			pthread_mutex_init(&mutex_, &mutexattr);
			pthread_mutexattr_destroy(&mutexattr);
		}

		RecursiveMutex::~RecursiveMutex()
		{
			pthread_mutex_destroy(&mutex_);
		}

		void RecursiveMutex::lock()
		{
			pthread_mutex_lock(&mutex_);
			if (count_ == 0)
			{
				owner_ = currentTid();
			}
			count_ += 1;
		}

		bool RecursiveMutex::tryLock()
		{
			if (pthread_mutex_trylock(&mutex_) == 0)
			{
				if (count_ == 0)
				{
					owner_ = currentTid();
				}
				count_ += 1;
				return true;
			}
			return false;
		}

		void RecursiveMutex::unlock()
		{
			if (count_ > 0)
			{
				if (count_ == 1)
				{
					owner_ = 0;
				}
				count_ -= 1;
				pthread_mutex_unlock(&mutex_);
			}
		}

		bool RecursiveMutex::isLockedByCurrentThread() const
		{
			return (owner_ == currentTid());
		}
	} // namespace base
} // namespace nets