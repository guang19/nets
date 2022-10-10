//
// Created by guang19
//

#ifndef NETS_TIMER_H
#define NETS_TIMER_H

#include <atomic>
#include <functional>

#include "nets/base/Noncopyable.h"
#include "nets/base/Timestamp.h"

namespace nets
{
    struct TimerId : public Copyable
    {
    public:
        using IdType = ::int64_t;

    public:
        TimerId();
        explicit TimerId(IdType key, const Timestamp& value);
        ~TimerId() = default;

        TimerId(const TimerId& other);
        TimerId(TimerId&& other) noexcept;
        TimerId& operator=(const TimerId& other);
        TimerId& operator=(TimerId&& other) noexcept;

        bool operator==(const TimerId& other) const;

        IdType key_;
        Timestamp value_;
    };

    class Timer : Noncopyable
    {
    public:
        using TimeType = ::time_t;
        using IdType = TimerId::IdType;
        using TimerCallback = ::std::function<void()>;
        static constexpr ::int32_t kRepeatForever = -1;

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
            return repeatTimes_ == kRepeatForever || repeatTimes_ > 0;
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
        TimerId id_;
        ::int32_t repeatTimes_;
        // unit: milliseconds
        TimeType interval_;
        bool fixedDelay_;
        TimerCallback timerCallback_;

        // not thread-safe, because there is usually only one TimerManager to manage Timer
        static ::std::atomic<IdType> timerIdGenerator_;
    };
} // namespace nets

#endif // NETS_TIMER_H