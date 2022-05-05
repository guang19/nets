//
// Created by guang19 on 2022/1/6.
//

#ifndef NETS_BASE_BLOCKINGQUEUE_H
#define NETS_BASE_BLOCKINGQUEUE_H

#include <ctime>
#include <deque>
#include <functional>
#include "nets/base/concurrency/ConditionVariable.h"
#include "nets/base/concurrency/Mutex.h"
#include "nets/base/Noncopyable.h"

namespace nets
{
    namespace base
    {
        template<typename T, typename Container = ::std::deque<T>>
        class BoundedBlockingQueue : Noncopyable
        {
            public:
				using ContainerType				= Container;
				using ValueType					= T;
				using LReferenceType			= ValueType&;
				using RReferenceType			= ValueType&&;
				using ConstReferenceType		= const ValueType&;
				using SizeType					= typename Container::size_type;
				using MutexType					= Mutex;
				using ConditionVariableType		= ConditionVariable;
				using LockGuardType				= LockGuard<MutexType>;

            public:
                explicit BoundedBlockingQueue(SizeType maxQueueSize) : maxQueueSize_(maxQueueSize)
				{
				}

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

				void popBack()
				{
					LockGuardType lock(mutex_);
					queue_.pop_back();
				}

				// notify blocking thread
                void notifyBlockingThread()
				{
					LockGuardType lock(mutex_);
					notFullCV_.notifyAll();
					notEmptyCV_.notifyAll();
				}

                void put(ConstReferenceType el);
                void put(RReferenceType el);
                void take(LReferenceType el);

				template<typename Predicate>
				bool put(ConstReferenceType el, Predicate p);
				template<typename Predicate>
				bool put(RReferenceType el, Predicate p);
                template<typename Predicate>
				bool take(LReferenceType el, Predicate p);

                bool put(ConstReferenceType el, ::std::time_t milliseconds);
                bool put(RReferenceType el, ::std::time_t milliseconds);
                bool take(LReferenceType el, ::std::time_t milliseconds);

                template<typename Predicate>
				bool put(ConstReferenceType el, ::std::time_t milliseconds, Predicate p);
				template<typename Predicate>
				bool put(RReferenceType el, ::std::time_t milliseconds, Predicate p);
                template<typename Predicate>
				bool take(LReferenceType el, ::std::time_t milliseconds, Predicate p);

                bool tryPush(ConstReferenceType el);
                bool tryPush(RReferenceType el);
                bool tryPop(LReferenceType el);

            private:
				bool isFull() const
                {
                    return queue_.size() >= maxQueueSize_;
                }

            private:
                SizeType maxQueueSize_ { 0 };
                ContainerType queue_ {};
                MutexType mutex_ {};
                ConditionVariableType notFullCV_ {};
				ConditionVariableType notEmptyCV_ {};
        };

		template<typename T, typename Container>
        void BoundedBlockingQueue<T, Container>::put(ConstReferenceType el)
        {
			LockGuardType lock(mutex_);
			while (isFull())
			{
				notFullCV_.wait(mutex_);
			}
            queue_.push_back(el);
			notEmptyCV_.notifyOne();
        }

		template<typename T, typename Container>
		void BoundedBlockingQueue<T, Container>::put(RReferenceType el)
		{
			LockGuardType lock(mutex_);
			while (isFull())
			{
				notFullCV_.wait(mutex_);
			}
			queue_.push_back(::std::forward<RReferenceType>(el));
			notEmptyCV_.notifyOne();
		}

		template<typename T, typename Container>
        void BoundedBlockingQueue<T, Container>::take(LReferenceType el)
        {
			LockGuardType lock(mutex_);
			while (queue_.empty())
			{
				notEmptyCV_.wait(mutex_);
			}
            el = queue_.front();
            queue_.pop_front();
			notFullCV_.notifyOne();
        }

		template<typename T, typename Container>
		template<typename Predicate>
		bool BoundedBlockingQueue<T, Container>::put(ConstReferenceType el, Predicate p)
		{
			LockGuardType lock(mutex_);
			while (isFull() && !p())
			{
				notFullCV_.wait(mutex_);
			}
			if (p())
			{
				return false;
			}
			queue_.push_back(el);
			notEmptyCV_.notifyOne();
			return true;
		}

		template<typename T, typename Container>
		template<typename Predicate>
		bool BoundedBlockingQueue<T, Container>::put(RReferenceType el, Predicate p)
		{
			LockGuardType lock(mutex_);
			while (isFull() && !p())
			{
				notFullCV_.wait(mutex_);
			}
			if (p())
			{
				return false;
			}
			queue_.push_back(::std::forward<RReferenceType>(el));
			notEmptyCV_.notifyOne();
			return true;
		}

		template<typename T, typename Container>
		template<typename Predicate>
		bool BoundedBlockingQueue<T, Container>::take(LReferenceType el, Predicate p)
		{
			LockGuardType lock(mutex_);
			while (queue_.empty() && !p())
			{
				notEmptyCV_.wait(mutex_);
			}
			if (p())
			{
				return false;
			}
			el = queue_.front();
			queue_.pop_front();
			notFullCV_.notifyOne();
			return true;
		}

		template<typename T, typename Container>
        bool BoundedBlockingQueue<T, Container>::put(ConstReferenceType el, ::std::time_t milliseconds)
        {
			LockGuardType lock(mutex_);
			if (isFull())
			{
				if (!notFullCV_.waitTimeout(mutex_, milliseconds))
				{
					return false;
				}
			}
			queue_.push_back(el);
			notEmptyCV_.notifyOne();
			return true;
        }

		template<typename T, typename Container>
		bool BoundedBlockingQueue<T, Container>::put(RReferenceType el, ::std::time_t milliseconds)
		{
			LockGuardType lock(mutex_);
			if (isFull())
			{
				if (!notFullCV_.waitTimeout(mutex_, milliseconds))
				{
					return false;
				}
			}
			queue_.push_back(::std::forward<RReferenceType>(el));
			notEmptyCV_.notifyOne();
			return true;
		}

		template<typename T, typename Container>
        bool BoundedBlockingQueue<T, Container>::take(LReferenceType el, ::std::time_t milliseconds)
        {
			LockGuardType lock(mutex_);
			if (queue_.empty())
			{
				if (!notEmptyCV_.waitTimeout(mutex_, milliseconds))
				{
					return false;
				}
			}
			el = queue_.front();
			queue_.pop_front();
			notFullCV_.notifyOne();
			return true;
        }

		template<typename T, typename Container>
		template<typename Predicate>
		bool BoundedBlockingQueue<T, Container>::put(ConstReferenceType el, ::std::time_t milliseconds, Predicate p)
		{
			LockGuardType lock(mutex_);
			if (isFull() && !p())
			{
				if(!notFullCV_.waitTimeout(mutex_, milliseconds))
				{
					return false;
				}
			}
			if (p())
			{
				return false;
			}
			queue_.push_back(el);
			notFullCV_.notifyOne();
			return true;
		}

		template<typename T, typename Container>
		template<typename Predicate>
		bool BoundedBlockingQueue<T, Container>::put(RReferenceType el, ::std::time_t milliseconds, Predicate p)
		{
			LockGuardType lock(mutex_);
			if (isFull() && !p())
			{
				if(!notFullCV_.waitTimeout(mutex_, milliseconds))
				{
					return false;
				}
			}
			if (p())
			{
				return false;
			}
			queue_.push_back(::std::forward<RReferenceType>(el));
			notFullCV_.notifyOne();
			return true;
		}

		template<typename T, typename Container>
		template<typename Predicate>
		bool BoundedBlockingQueue<T, Container>::take(LReferenceType el, ::std::time_t milliseconds, Predicate p)
		{
			LockGuardType lock(mutex_);
			if (queue_.empty() && !p())
			{
				if (!notEmptyCV_.waitTimeout(mutex_, milliseconds))
				{
					return false;
				}
			}
			if (p())
			{
				return false;
			}
			el = queue_.front();
			queue_.pop_front();
			notFullCV_.notifyOne();
			return true;
		}

		template<typename T, typename Container>
        bool BoundedBlockingQueue<T, Container>::tryPush(ConstReferenceType el)
		{
			LockGuardType lock(mutex_, TRY);
			if (lock.isLockedByCurrentThread() && !isFull())
			{
				queue_.push_back(el);
				notEmptyCV_.notifyOne();
				return true;
			}
			return false;
		}

		template<typename T, typename Container>
		bool BoundedBlockingQueue<T, Container>::tryPush(RReferenceType el)
		{
			LockGuardType lock(mutex_, TRY);
			if (lock.isLockedByCurrentThread() && !isFull())
			{
				queue_.push_back(::std::forward<RReferenceType>(el));
				notEmptyCV_.notifyOne();
				return true;
			}
			return false;
		}

		template<typename T, typename Container>
        bool BoundedBlockingQueue<T, Container>::tryPop(LReferenceType el)
		{
			LockGuardType lock(mutex_, TRY);
			if (lock.isLockedByCurrentThread() && !isFull())
			{
				el = queue_.front();
				queue_.pop_front();
				notFullCV_.notifyOne();
				return true;
			}
			return false;
		}
	} // namespace base
} // namespace nets

#endif // NETS_BASE_BLOCKINGQUEUE_H
