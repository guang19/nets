//
// Created by guang19 on 2022/1/6.
//

#ifndef NETS_BASE_BLOCKINGQUEUE_H
#define NETS_BASE_BLOCKINGQUEUE_H

#include <condition_variable>
#include <deque>
#include <mutex>
#include "base/noncopyable.h"

namespace nets
{
    namespace base
    {
        template<typename T>
        class BlockingQueue : noncopyable
        {
            public:
                using container_type          = std::deque<T>;
                using value_type              = T;
                using reference_type          = value_type&;
                using const_reference_type    = const value_type&;
                using size_type               = std::size_t;
                using milliseconds_type       = std::chrono::duration<std::size_t, std::milli>;
                using mutex_type              = std::recursive_mutex;
                using condition_variable_type = std::condition_variable_any;
                using lock_guard_type         = std::lock_guard<std::recursive_mutex>;
                using unique_lock_type        = std::unique_lock<std::recursive_mutex>;

            public:
                explicit BlockingQueue(size_type maxQueueSize) : maxQueueSize_(maxQueueSize)
                {}
                ~BlockingQueue() = default;

                bool empty()
                {
                    lock_guard_type lock(mtx_);
                    return queue_.empty();
                }

                size_type size()
                {
                    lock_guard_type lock(mtx_);
                    return queue_.size();
                }

                void put(const_reference_type el);
                void take(reference_type el);

				template<typename Predicate>
				void put(const_reference_type el,  Predicate p);
				template<typename Predicate>
				void take(reference_type el, Predicate p);

                bool put(const_reference_type el, const milliseconds_type& time);
                bool take(reference_type el, const milliseconds_type& time);

				template<typename Predicate>
				bool put(const_reference_type el, const milliseconds_type& time, Predicate p);
				template<typename Predicate>
				bool take(reference_type el, const milliseconds_type& time, Predicate p);

                bool tryPush(const_reference_type el);
                bool tryPop(reference_type el);

            private:
                bool full() const
                {
                    return maxQueueSize_ > 0 && queue_.size() >= maxQueueSize_;
                }

            private:
                size_type maxQueueSize_;
                container_type queue_;
                mutex_type mtx_;
                condition_variable_type notFullCv_;
                condition_variable_type notEmptyCv_;
        };

        template<typename T>
        void BlockingQueue<T>::put(const_reference_type el)
        {
            unique_lock_type lock(mtx_);
            notFullCv_.wait(lock, [&]
            {
               return !full();
            });
            queue_.push_back(el);
            notEmptyCv_.notify_one();
        }

        template<typename T>
        void BlockingQueue<T>::take(reference_type el)
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

		template<typename T>
		template<typename Predicate>
		void BlockingQueue<T>::put(const_reference_type el,  Predicate p)
		{
			unique_lock_type lock(mtx_);
			notFullCv_.wait(lock, [&]
			{
				return !full() || !p();
			});
			queue_.push_back(el);
			notEmptyCv_.notify_one();
		}

		template<typename T>
		template<typename Predicate>
		void BlockingQueue<T>::take(reference_type el, Predicate p)
		{
			unique_lock_type lock(mtx_);
			notEmptyCv_.wait(lock, [&]
			{
				return !queue_.empty() || !p();
			});
			el = queue_.front();
			queue_.pop_front();
			notFullCv_.notify_one();
		}

        template<typename T>
        bool BlockingQueue<T>::put(const_reference_type el, const milliseconds_type& time)
        {
            unique_lock_type lock(mtx_);
            if (notFullCv_.wait_for(lock, time, [&]
            {
                return !full();
            }))
            {
                queue_.push_back(el);
                notEmptyCv_.notify_one();
                return true;
            }
            return false;
        }

        template<typename T>
        bool BlockingQueue<T>::take(reference_type el, const milliseconds_type& time)
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

		template<typename T>
		template<typename Predicate>
		bool BlockingQueue<T>::put(const_reference_type el, const milliseconds_type& time, Predicate p)
		{
			unique_lock_type lock(mtx_);
			if (notFullCv_.wait_for(lock, time, [&]
			{
				return !full() || !p();
			}))
			{
				queue_.push_back(el);
				notEmptyCv_.notify_one();
				return true;
			}
			return false;
		}

		template<typename T>
		template<typename Predicate>
		bool BlockingQueue<T>::take(reference_type el, const milliseconds_type& time, Predicate p)
		{
			unique_lock_type lock(mtx_);
			if (notEmptyCv_.wait_for(lock, time, [&]
			{
				return !queue_.empty() || !p();
			}))
			{
				el = queue_.front();
				queue_.pop_front();
				notFullCv_.notify_one();
				return true;
			}
			return false;
		}

        template<typename T>
        bool BlockingQueue<T>::tryPush(const_reference_type el)
		{
			unique_lock_type lock(mtx_, std::try_to_lock);
			if (lock.owns_lock() && !full())
			{
				queue_.push_back(el);
				notEmptyCv_.notify_one();
				return true;
			}
			return false;
		}

		template<typename T>
        bool BlockingQueue<T>::tryPop(reference_type el)
		{
			unique_lock_type lock(mtx_, std::try_to_lock);
			if (lock.owns_lock() && !full())
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
