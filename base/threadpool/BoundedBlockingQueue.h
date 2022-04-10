//
// Created by guang19 on 2022/1/6.
//

#ifndef NETS_BASE_BLOCKINGQUEUE_H
#define NETS_BASE_BLOCKINGQUEUE_H

#include <condition_variable>
#include <deque>
#include <functional>
#include <mutex>
#include "base/Noncopyable.h"

namespace nets
{
    namespace base
    {
        template<typename T, typename Container = ::std::deque<T>>
        class BoundedBlockingQueue : Noncopyable
        {
            public:
				using container_type          = Container;
                using value_type              = T;
                using reference_type          = value_type&;
                using const_reference_type    = const value_type&;
                using size_type               = typename Container::size_type;
                using milliseconds_type       = ::std::chrono::milliseconds;
                using mutex_type              = ::std::recursive_mutex;
                using condition_variable_type = ::std::condition_variable_any;
                using lock_guard_type         = ::std::lock_guard<::std::recursive_mutex>;
                using unique_lock_type        = ::std::unique_lock<::std::recursive_mutex>;

            public:
                explicit BoundedBlockingQueue(size_type maxQueueSize) : maxQueueSize_(maxQueueSize)
				{}
				~BoundedBlockingQueue() = default;

                bool isEmpty()
                {
                    lock_guard_type lock(mtx_);
                    return queue_.empty();
                }

                size_type size()
                {
                    lock_guard_type lock(mtx_);
                    return queue_.size();
                }

				void pop_back()
				{
					lock_guard_type lock(mtx_);
					queue_.pop_back();
				}

                // 唤醒put线程和take线程
                void notifyBlockingThread()
				{
					lock_guard_type lock(mtx_);
					notFullCv_.notify_all();
					notEmptyCv_.notify_all();
				}

                void put(const_reference_type el);
                void take(reference_type el);

				/********************************************************************************
				 *
				 * @Predicate : 当Predicate满足时，put或take操作不会阻塞线程并且put和take操作一定失败
				 *
				 ********************************************************************************/
				template<typename Predicate>
				bool put(const_reference_type el,  Predicate p);
                template<typename Predicate>
				bool take(reference_type el, Predicate p);

                bool put(const_reference_type el, const milliseconds_type& time);
                bool take(reference_type el, const milliseconds_type& time);

                template<typename Predicate>
				bool put(const_reference_type el, const milliseconds_type& time, Predicate p);
                template<typename Predicate>
				bool take(reference_type el, const milliseconds_type& time, Predicate p);

                bool tryPush(const_reference_type el);
                bool tryPop(reference_type el);

            private:
                // 内部使用，能够确保使用前已加锁
				bool isFull() const
                {
                    return queue_.size() >= maxQueueSize_;
                }

            private:
                size_type maxQueueSize_ { 0 };
                container_type queue_ {};
                mutex_type mtx_ {};
                condition_variable_type notFullCv_ {};
                condition_variable_type notEmptyCv_ {};
        };

		template<typename T, typename Container>
        void BoundedBlockingQueue<T, Container>::put(const_reference_type el)
        {
			unique_lock_type lock(mtx_);
            notFullCv_.wait(lock, [&]
            {
               return !isFull();
            });
            queue_.push_back(el);
            notEmptyCv_.notify_one();
        }

		template<typename T, typename Container>
        void BoundedBlockingQueue<T, Container>::take(reference_type el)
        {
			unique_lock_type lock(mtx_);
            notEmptyCv_.wait(lock, [&]
            {
                return !queue_.empty();
            });
            el = queue_.front();
            queue_.pop_front();
            notFullCv_.notify_one();
        }

		template<typename T, typename Container>
		template<typename Predicate>
		bool BoundedBlockingQueue<T, Container>::put(const_reference_type el, Predicate p)
		{
			unique_lock_type lock(mtx_);
			notFullCv_.wait(lock, [&]
			{
				return !isFull() || p();
			});
			// 因为wait的条件是 (!full() || p())，所以并不确定是否full，还需再判断一次
			if (isFull() || p())
			{
				return false;
			}
			queue_.push_back(el);
			notEmptyCv_.notify_one();
			return true;
		}

		template<typename T, typename Container>
		template<typename Predicate>
		bool BoundedBlockingQueue<T, Container>::take(reference_type el, Predicate p)
		{
			unique_lock_type lock(mtx_);
			notEmptyCv_.wait(lock, [&]
			{
				return !queue_.empty() || p();
			});
			if (queue_.empty() || p())
			{
				return false;
			}
			el = queue_.front();
			queue_.pop_front();
			notFullCv_.notify_one();
			return true;
		}

		template<typename T, typename Container>
        bool BoundedBlockingQueue<T, Container>::put(const_reference_type el, const milliseconds_type& time)
        {
			unique_lock_type lock(mtx_);
            if (notFullCv_.wait_for(lock, time, [&]
            {
                return !isFull();
            }))
            {
                queue_.push_back(el);
                notEmptyCv_.notify_one();
                return true;
            }
            return false;
        }

		template<typename T, typename Container>
        bool BoundedBlockingQueue<T, Container>::take(reference_type el, const milliseconds_type& time)
        {
			unique_lock_type lock(mtx_);
            if (notEmptyCv_.wait_for(lock, time, [&]
            {
                return !queue_.empty();
            }))
            {
                el = queue_.front();
                queue_.pop_front();
                notFullCv_.notify_one();
                return true;
            }
            return false;
        }

		template<typename T, typename Container>
		template<typename Predicate>
		bool BoundedBlockingQueue<T, Container>::put(const_reference_type el, const milliseconds_type& time, Predicate p)
		{
			unique_lock_type lock(mtx_);
			if (notFullCv_.wait_for(lock, time, [&]
			{
				return !isFull() || p();
			}))
			{
				if (isFull() || p())
				{
					return false;
				}
				queue_.push_back(el);
				notEmptyCv_.notify_one();
				return true;
			}
			return false;
		}

		template<typename T, typename Container>
		template<typename Predicate>
		bool BoundedBlockingQueue<T, Container>::take(reference_type el, const milliseconds_type& time, Predicate p)
		{
			unique_lock_type lock(mtx_);
			if (notEmptyCv_.wait_for(lock, time, [&]
			{
				return !queue_.empty() || p();
			}))
			{
				if (queue_.empty() || p())
				{
					return false;
				}
				el = queue_.front();
				queue_.pop_front();
				notFullCv_.notify_one();
				return true;
			}
			return false;
		}

		template<typename T, typename Container>
        bool BoundedBlockingQueue<T, Container>::tryPush(const_reference_type el)
		{
			unique_lock_type lock(mtx_, ::std::try_to_lock);
			if (lock.owns_lock() && !isFull())
			{
				queue_.push_back(el);
				notEmptyCv_.notify_one();
				return true;
			}
			return false;
		}

		template<typename T, typename Container>
        bool BoundedBlockingQueue<T, Container>::tryPop(reference_type el)
		{
			unique_lock_type lock(mtx_, ::std::try_to_lock);
			if (lock.owns_lock() && !isFull())
			{
				el = queue_.front();
				queue_.pop_front();
				notFullCv_.notify_one();
				return true;
			}
			return false;
		}
    } // namespace base
} // namespace nets

#endif // NETS_BASE_BLOCKINGQUEUE_H
