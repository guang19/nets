//
// Created by guang19 on 2022/1/6.
//

#ifndef NETS_BASE_BLOCKINGQUEUE_H
#define NETS_BASE_BLOCKINGQUEUE_H

#include <condition_variable>
#include <ctime>
#include <deque>
#include <functional>
#include <mutex>

#include "nets/base/concurrency/ConditionVariable.h"
#include "nets/base/concurrency/Mutex.h"
#include "nets/base/Noncopyable.h"

namespace nets::base
{
	template <typename T>
	class BoundedBlockingQueue : Noncopyable
	{
	public:
		using ValueType = T;
		using LReferenceType = ValueType&;
		using RReferenceType = ValueType&&;
		using ConstReferenceType = const ValueType&;
		using MutexType = ::std::mutex;
		using LockGuardType = ::std::lock_guard<MutexType>;
		using UniqueLockType = ::std::unique_lock<MutexType>;
		using ConditionVariableType = ::std::condition_variable;
		using TimeType = ::time_t;
		using MillisTimeType = ::std::chrono::milliseconds;
		using ContainerType = ::std::deque<ValueType>;
		using SizeType = typename ContainerType::size_type;
		using PredicateType = ::std::function<bool()>;

	public:
		const static SizeType DefaultMaxQueueSize = INT32_MAX - 1;

		BoundedBlockingQueue() : maxQueueSize_(DefaultMaxQueueSize) {}
		explicit BoundedBlockingQueue(SizeType maxQueueSize) : maxQueueSize_(maxQueueSize) {}

		~BoundedBlockingQueue() = default;

		bool isEmpty()
		{
			LockGuardType lock(mutex_);
			return queue_.empty();
		}

		SizeType size()
		{
			LockGuardType lock(mutex_);
			return queue_.size();
		}

		void popFront()
		{
			LockGuardType lock(mutex_);
			queue_.pop_front();
		}

		// notify blocking thread
		void notifyBlockingThread()
		{
			LockGuardType lock(mutex_);
			notFullCV_.notify_all();
			notEmptyCV_.notify_all();
		}

		void put(RReferenceType el);
		void put(ConstReferenceType el);
		void take(LReferenceType el);

		bool put(RReferenceType el, PredicateType p);
		bool put(ConstReferenceType el, PredicateType p);
		bool take(LReferenceType el, PredicateType p);

		bool put(RReferenceType el, TimeType milliseconds);
		bool put(ConstReferenceType el, TimeType milliseconds);
		bool take(LReferenceType el, TimeType milliseconds);

		bool put(RReferenceType el, TimeType milliseconds, PredicateType p);
		bool put(ConstReferenceType el, TimeType milliseconds, PredicateType p);
		bool take(LReferenceType el, TimeType milliseconds, PredicateType p);

		bool tryPush(RReferenceType el);
		bool tryPush(ConstReferenceType el);
		bool tryPop(LReferenceType el);

	private:
		bool isFull() const
		{
			return queue_.size() >= maxQueueSize_;
		}

	private:
		SizeType maxQueueSize_ {0};
		ContainerType queue_ {};
		MutexType mutex_ {};
		ConditionVariableType notFullCV_ {};
		ConditionVariableType notEmptyCV_ {};
	};

	template <typename T>
	void BoundedBlockingQueue<T>::put(RReferenceType el)
	{
		UniqueLockType lock(mutex_);
		notFullCV_.wait(lock,
						[&]() -> bool
						{
							return !isFull();
						});
		queue_.push_back(::std::forward<RReferenceType>(el));
		notEmptyCV_.notify_one();
	}

	template <typename T>
	void BoundedBlockingQueue<T>::put(ConstReferenceType el)
	{
		UniqueLockType lock(mutex_);
		notFullCV_.wait(lock,
						[&]() -> bool
						{
							return !isFull();
						});
		queue_.push_back(el);
		notEmptyCV_.notify_one();
	}

	template <typename T>
	void BoundedBlockingQueue<T>::take(LReferenceType el)
	{
		UniqueLockType lock(mutex_);
		notFullCV_.wait(lock,
						[&]() -> bool
						{
							return !queue_.empty();
						});
		el = queue_.front();
		queue_.pop_front();
		notFullCV_.notify_one();
	}

	template <typename T>
	bool BoundedBlockingQueue<T>::put(RReferenceType el, PredicateType p)
	{
		UniqueLockType lock(mutex_);
		notFullCV_.wait(lock,
						[&]() -> bool
						{
							return (!isFull() || p());
						});
		if (p())
		{
			return false;
		}
		queue_.push_back(::std::forward<RReferenceType>(el));
		notEmptyCV_.notify_one();
		return true;
	}

	template <typename T>
	bool BoundedBlockingQueue<T>::put(ConstReferenceType el, PredicateType p)
	{
		UniqueLockType lock(mutex_);
		notFullCV_.wait(lock,
						[&]() -> bool
						{
							return (!isFull() || p());
						});
		if (p())
		{
			return false;
		}
		queue_.push_back(el);
		notEmptyCV_.notify_one();
		return true;
	}

	template <typename T>
	bool BoundedBlockingQueue<T>::take(LReferenceType el, PredicateType p)
	{
		UniqueLockType lock(mutex_);
		notFullCV_.wait(lock,
						[&]() -> bool
						{
							return (!queue_.empty() || p());
						});
		if (p())
		{
			return false;
		}
		el = queue_.front();
		queue_.pop_front();
		notFullCV_.notify_one();
		return true;
	}

	template <typename T>
	bool BoundedBlockingQueue<T>::put(RReferenceType el, TimeType milliseconds)
	{
		UniqueLockType lock(mutex_);
		notFullCV_.wait_for(
			lock,
			[&]() -> bool
			{
				return !isFull();
			},
			MillisTimeType(milliseconds));
		queue_.push_back(::std::forward<RReferenceType>(el));
		notEmptyCV_.notify_one();
		return true;
	}

	template <typename T>
	bool BoundedBlockingQueue<T>::put(ConstReferenceType el, TimeType milliseconds)
	{
		UniqueLockType lock(mutex_);
		notFullCV_.wait_for(
			lock,
			[&]() -> bool
			{
				return !isFull();
			},
			MillisTimeType(milliseconds));
		queue_.push_back(el);
		notEmptyCV_.notify_one();
		return true;
	}

	template <typename T>
	bool BoundedBlockingQueue<T>::take(LReferenceType el, TimeType milliseconds)
	{
		UniqueLockType lock(mutex_);
		notFullCV_.wait_for(lock,
						[&]() -> bool
						{
							return !queue_.empty();
						}, MillisTimeType(milliseconds));
		el = queue_.front();
		queue_.pop_front();
		notFullCV_.notify_one();
		return true;
	}

	template <typename T>
	bool BoundedBlockingQueue<T>::put(RReferenceType el, TimeType milliseconds, PredicateType p)
	{
		UniqueLockType lock(mutex_);
		notFullCV_.wait_for(
			lock,
			[&]() -> bool
			{
				return (!isFull() || p());
			},
			MillisTimeType(milliseconds));
		if (p())
		{
			return false;
		}
		queue_.push_back(::std::forward<RReferenceType>(el));
		notFullCV_.notify_one();
		return true;
	}

	template <typename T>
	bool BoundedBlockingQueue<T>::put(ConstReferenceType el, TimeType milliseconds, PredicateType p)
	{
		UniqueLockType lock(mutex_);
		if(notFullCV_.wait_for(
			lock,
			[&]() -> bool
			{
				return (!isFull() || p());
			},
			MillisTimeType(milliseconds)))
		{
			if (p())
			{
				return false;
			}
			queue_.push_back(el);
			notFullCV_.notify_one();
			return true;
		}
		return false;
	}

	template <typename T>
	bool BoundedBlockingQueue<T>::take(LReferenceType el, TimeType milliseconds, PredicateType p)
	{
		UniqueLockType lock(mutex_);
		if (notEmptyCV_.wait_for(
			lock,
			[&]()
			{
				return (!queue_.empty() || p());
			},
			MillisTimeType(milliseconds)))
		{
			el = queue_.front();
			queue_.pop_front();
			notFullCV_.notify_one();
			return true;
		}
		return false;
	}

	template <typename T>
	bool BoundedBlockingQueue<T>::tryPush(RReferenceType el)
	{
		UniqueLockType lock(mutex_, ::std::try_to_lock);
		if (lock.owns_lock() && !isFull())
		{
			queue_.push_back(::std::forward<RReferenceType>(el));
			notEmptyCV_.notify_one();
			return true;
		}
		return false;
	}

	template <typename T>
	bool BoundedBlockingQueue<T>::tryPush(ConstReferenceType el)
	{
		UniqueLockType lock(mutex_, ::std::try_to_lock);
		if (lock.owns_lock() && !isFull())
		{
			queue_.push_back(el);
			notEmptyCV_.notify_one();
			return true;
		}
		return false;
	}

	template <typename T>
	bool BoundedBlockingQueue<T>::tryPop(LReferenceType el)
	{
		UniqueLockType lock(mutex_, ::std::try_to_lock);
		if (lock.owns_lock() && !isEmpty())
		{
			el = queue_.front();
			queue_.pop_front();
			notFullCV_.notify_one();
			return true;
		}
		return false;
	}
} // namespace nets::base

#endif // NETS_BASE_BLOCKINGQUEUE_H
