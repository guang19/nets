//
// Created by YangGuang on 2022/5/2.
//

#ifndef NETS_MUTEX_H
#define NETS_MUTEX_H

#include <cstdint>
#include <pthread.h>
#include "nets/base/Noncopyable.h"

namespace nets
{
	namespace base
	{
		class Mutex : Noncopyable
		{
			public:
				Mutex();
				~Mutex();

			public:
				void lock();
				bool tryLock();
				void unlock();
				bool isLockedByCurrentThread() const;

				inline ::pthread_mutex_t* getMutexPtr()
				{
					return &mutex_;
				}

			public:
				typedef struct OwnerGuard_
				{
					public:
						explicit OwnerGuard_(Mutex& mutex);
						~OwnerGuard_();

					private:
						Mutex& mutex_;
				} OwnerGuard;

			private:
				::pthread_mutex_t mutex_ {};
				::pid_t owner_ { 0 };
		};

		class RecursiveMutex : Noncopyable
		{
			public:
				RecursiveMutex();
				~RecursiveMutex();

			public:
				void lock();
				bool tryLock();
				void unlock();
				bool isLockedByCurrentThread() const;

				inline ::pthread_mutex_t* getMutexPtr()
				{
					return &mutex_;
				}

			private:
				::pthread_mutex_t mutex_ {};
				::pid_t owner_ { 0 };
				uint16_t count_ { 0 };
		};

		enum LockType
		{
			NOW,
			TRY,
			DEFER
		};

		template <class MutexType>
		class LockGuard : Noncopyable
		{
			public:
				explicit LockGuard(MutexType& mutex) : LockGuard(mutex, LockType::NOW)
				{
				}

				LockGuard(MutexType& mutex, LockType lockType) : mutex_(mutex), state_(false)
				{
					switch (lockType)
					{
						case LockType::NOW:
							mutex_.lock();
							state_ = true;
							break;
						case LockType::TRY:
							state_ = mutex_.tryLock();
							break;
						case LockType::DEFER:
							state_ = false;
							break;
					}
				}

				inline void lock()
				{
					if (!state_)
					{
						mutex_.lock();
						state_ = true;
					}
				}

				inline void tryLock()
				{
					if (!state_)
					{
						state_ = mutex_.tryLock();
					}
				}

				inline void unlock()
				{
					if (state_)
					{
						mutex_.unlock();
						state_ = false;
					}
				}

				inline bool isLockedByCurrentThread()
				{
					return state_;
				}

				~LockGuard()
				{
					unlock();
				}

			private:
				MutexType& mutex_;
				bool state_ { false };
		};
	} // namespace base
} // namespace nets

#endif //NETS_MUTEX_H