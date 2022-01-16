//
// Created by guang19 on 2022/1/6.
//

#ifndef NETS_BASE_BLOCKINGQUEUE_H
#define NETS_BASE_BLOCKINGQUEUE_H

#include <condition_variable>
#include <mutex>
#include <deque>
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
                using reference_type          = T&;
                using const_reference_type    = const T&;
                using size_type               = std::size_t;
                using condition_variable_type = std::condition_variable;
                using lock_guard_type         = std::lock_guard<std::mutex>;
                using unique_lock_type        = std::unique_lock<std::mutex>;

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

            private:
                bool full() const
                {
                    return maxQueueSize_ > 0 && queue_.size() >= maxQueueSize_;
                }

            private:
                size_type maxQueueSize_;
                container_type queue_;
                std::mutex mtx_;
                condition_variable_type notFullCV_;
                condition_variable_type notEmptyCV_;
        };

        template<typename T>
        void BlockingQueue<T>::put(const_reference_type el)
        {
            unique_lock_type lock(mtx_);
            notFullCV_.wait(lock, [&]
            {
               return !full();
            });
            queue_.push_back(el);
            notEmptyCV_.notify_one();
        }

        template<typename T>
        void BlockingQueue<T>::take(reference_type el)
        {
            unique_lock_type lock(mtx_);
            notEmptyCV_.wait(lock, [&]
            {
                return !queue_.empty();
            });
            el = queue_.front();
            queue_.pop_front();
            notFullCV_.notify_one();
        }
    } // namespace base
} // namespace nets

#endif // NETS_BASE_BLOCKINGQUEUE_H
