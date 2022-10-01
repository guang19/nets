//
// Created by guang19
//

#ifndef NETS_BOUNDED_BLOCKING_QUEUE_H
#define NETS_BOUNDED_BLOCKING_QUEUE_H

#include <condition_variable>
#include <deque>
#include <functional>
#include <shared_mutex>

#include "nets/base/Noncopyable.h"

namespace nets
{
    template <typename T>
    class BoundedBlockingQueue : Noncopyable
    {
    public:
        using ValueType = T;
        using LReferenceType = ValueType&;
        using RReferenceType = ValueType&&;
        using ConstReferenceType = const ValueType&;
        using MutexType = ::std::shared_mutex;
        using SharedLockType = ::std::shared_lock<MutexType>;
        using UniqueLockType = ::std::unique_lock<MutexType>;
        using ConditionVariableType = ::std::condition_variable_any;
        using TimeType = ::time_t;
        using MillisTimeType = ::std::chrono::milliseconds;
        using ContainerType = ::std::deque<ValueType>;
        using SizeType = typename ContainerType::size_type;
        using PredicateType = ::std::function<bool()>;

    public:
        explicit BoundedBlockingQueue(SizeType maxQueueSize = gDefaultMaxQueueSize) : maxQueueSize_(maxQueueSize) {}
        ~BoundedBlockingQueue() = default;

        bool isEmpty()
        {
            SharedLockType lock(mutex_);
            return queue_.empty();
        }

        SizeType size()
        {
            SharedLockType lock(mutex_);
            return queue_.size();
        }

        // notify blocking thread
        void notifyBlockingThread()
        {
            UniqueLockType lock(mutex_);
            notFullCV_.notify_all();
            notEmptyCV_.notify_all();
        }

        void put(RReferenceType el);
        void put(ConstReferenceType el);
        void take(LReferenceType el);

        bool put(RReferenceType el, const PredicateType& p);
        bool put(ConstReferenceType el, const PredicateType& p);
        bool take(LReferenceType el, const PredicateType& p);

        bool put(RReferenceType el, TimeType milliseconds);
        bool put(ConstReferenceType el, TimeType milliseconds);
        bool take(LReferenceType el, TimeType milliseconds);

        bool put(RReferenceType el, TimeType milliseconds, const PredicateType& p);
        bool put(ConstReferenceType el, TimeType milliseconds, const PredicateType& p);
        bool take(LReferenceType el, TimeType milliseconds, const PredicateType& p);

        bool tryPush(RReferenceType el);
        bool tryPush(ConstReferenceType el);
        bool tryPop(LReferenceType el);

    private:
        inline bool isFull() const
        {
            return queue_.size() >= maxQueueSize_;
        }

    private:
        SizeType maxQueueSize_ {0};
        ContainerType queue_ {};
        MutexType mutex_ {};
        ConditionVariableType notFullCV_ {};
        ConditionVariableType notEmptyCV_ {};

        static constexpr SizeType gDefaultMaxQueueSize = INT32_MAX;
    };

    template <typename T>
    void BoundedBlockingQueue<T>::put(RReferenceType el)
    {
        UniqueLockType lock(mutex_);
        notFullCV_.wait(lock,
                        [this]() -> bool
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
                        [this]() -> bool
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
        notEmptyCV_.wait(lock,
                         [this]() -> bool
                         {
                             return !queue_.empty();
                         });
        el = queue_.front();
        queue_.pop_front();
        notFullCV_.notify_one();
    }

    template <typename T>
    bool BoundedBlockingQueue<T>::put(RReferenceType el, const PredicateType& p)
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
    bool BoundedBlockingQueue<T>::put(ConstReferenceType el, const PredicateType& p)
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
    bool BoundedBlockingQueue<T>::take(LReferenceType el, const PredicateType& p)
    {
        UniqueLockType lock(mutex_);
        notEmptyCV_.wait(lock,
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
        if (notFullCV_.wait_for(lock, MillisTimeType(milliseconds),
                                [this]() -> bool
                                {
                                    return !isFull();
                                }))
        {
            queue_.push_back(::std::forward<RReferenceType>(el));
            notEmptyCV_.notify_one();
            return true;
        }
        return false;
    }

    template <typename T>
    bool BoundedBlockingQueue<T>::put(ConstReferenceType el, TimeType milliseconds)
    {
        UniqueLockType lock(mutex_);
        if (notFullCV_.wait_for(lock, MillisTimeType(milliseconds),
                                [this]() -> bool
                                {
                                    return !isFull();
                                }))
        {
            queue_.push_back(el);
            notEmptyCV_.notify_one();
            return true;
        }
        return false;
    }

    template <typename T>
    bool BoundedBlockingQueue<T>::take(LReferenceType el, TimeType milliseconds)
    {
        UniqueLockType lock(mutex_);
        if (notEmptyCV_.wait_for(lock, MillisTimeType(milliseconds),
                                 [this]() -> bool
                                 {
                                     return !queue_.empty();
                                 }))
        {
            el = queue_.front();
            queue_.pop_front();
            notFullCV_.notify_one();
            return true;
        }
        return false;
    }

    template <typename T>
    bool BoundedBlockingQueue<T>::put(RReferenceType el, TimeType milliseconds, const PredicateType& p)
    {
        UniqueLockType lock(mutex_);
        if (notFullCV_.wait_for(lock, MillisTimeType(milliseconds),
                                [&]() -> bool
                                {
                                    return (!isFull() || p());
                                }))
        {
            if (p())
            {
                return false;
            }
            queue_.push_back(::std::forward<RReferenceType>(el));
            notEmptyCV_.notify_one();
            return true;
        }
        return false;
    }

    template <typename T>
    bool BoundedBlockingQueue<T>::put(ConstReferenceType el, TimeType milliseconds, const PredicateType& p)
    {
        UniqueLockType lock(mutex_);
        if (notFullCV_.wait_for(lock, MillisTimeType(milliseconds),
                                [&]() -> bool
                                {
                                    return (!isFull() || p());
                                }))
        {
            if (p())
            {
                return false;
            }
            queue_.push_back(el);
            notEmptyCV_.notify_one();
            return true;
        }
        return false;
    }

    template <typename T>
    bool BoundedBlockingQueue<T>::take(LReferenceType el, TimeType milliseconds, const PredicateType& p)
    {
        UniqueLockType lock(mutex_);
        if (notEmptyCV_.wait_for(lock, MillisTimeType(milliseconds),
                                 [&]()
                                 {
                                     return (!queue_.empty() || p());
                                 }))
        {
            if (p())
            {
                return false;
            }
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
        if (lock.owns_lock() && !queue_.empty())
        {
            el = queue_.front();
            queue_.pop_front();
            notFullCV_.notify_one();
            return true;
        }
        return false;
    }
} // namespace nets

#endif // NETS_BOUNDED_BLOCKING_QUEUE_H