//
// Created by guang19
//

#ifndef NETS_BASE_TIMER_H
#define NETS_BASE_TIMER_H

#include <atomic>
#include <functional>

#include "nets/base/Noncopyable.h"
#include "nets/base/Timestamp.h"

namespace nets::base
{
    class Timer : Noncopyable
    {
    public:
        using TimeType = ::time_t;
        using IdType = ::int64_t;
        using TimerCallback = ::std::function<void()>;
        static constexpr ::int32_t RepeatForever = -1;

    public:
        class TimerId : public Copyable
        {
        public:
            TimerId();
            explicit TimerId(IdType key, const Timestamp& value);
            ~TimerId() = default;

            TimerId(const TimerId& other);
            TimerId(TimerId&& other) noexcept;
            TimerId& operator=(const TimerId& other);
            TimerId& operator=(TimerId&& other) noexcept;

            inline bool operator==(const TimerId& other) const
            {
                return key_ == other.key_ && value_ == other.value_;
            }

        public:
            IdType key_ {-1};
            Timestamp value_ {0};
        };

    public:
        explicit Timer(const Timestamp& expiredTime, ::int32_t repeatTimes, TimeType interval, bool fixedDelay = false);
        ~Timer() = default;

        // in order to allow map store timer, needs to support move
        Timer(Timer&& other) noexcept;
        Timer& operator=(Timer&& other) noexcept;

        inline const TimerId& id() const
        {
            return id_;
        }

        inline const Timestamp& expiredTime() const
        {
            return id_.value_;
        }

        inline void setExpiredTime(const Timestamp& expiredTime)
        {
            id_.value_ = expiredTime;
        }

        inline bool isRepeatable() const
        {
            return repeatTimes_ == RepeatForever || repeatTimes_ > 0;
        }

        inline void setRepeatTimes(::int32_t repeatTimes)
        {
            repeatTimes_ = repeatTimes;
        }

        inline bool isExpired(const Timestamp& now) const
        {
            return now >= id_.value_;
        }

        template <typename Fn, typename... Args>
        inline void setTimerCallback(Fn&& fn, Args&&... args)
        {
            timerCallback_ = ::std::bind(::std::forward<Fn>(fn), ::std::forward<Args>(args)...);
        }

        void onTimer(const Timestamp& now);

    private:
        TimerId id_ {};
        ::int32_t repeatTimes_ {0};
        // unit: milliseconds
        TimeType interval_ {0};
        bool fixedDelay_ {false};
        TimerCallback timerCallback_ {};

        // not thread-safe, because there is usually only one TimerManager to manage Timer
        static ::std::atomic<IdType> TimerIdGenerator;
    };
} // namespace nets::base

#endif // NETS_BASE_TIMER_H