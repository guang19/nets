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

    public:
        Timer();
        explicit Timer(const Timestamp& expiredTime);
        explicit Timer(const Timestamp& expiredTime, TimeType interval);
        explicit Timer(const Timestamp& expiredTime, ::int32_t repeatTimes, TimeType interval);

        // in order to allow map store Timer, needs to support move
        Timer(Timer&& other) noexcept;
        Timer& operator=(Timer&& other) noexcept;

        ~Timer() = default;

        IdType id() const
        {
            return id_;
        }

        const Timestamp& expiredTime() const
        {
            return expiredTime_;
        }

        template <typename Fn, typename... Args>
        void setTimerCallback(Fn&& fn, Args&&... args)
        {
            timerCallback_ = ::std::bind(::std::forward<Fn>(fn), ::std::forward<Args>(args)...);
        }

        void onTimer()
        {
            if (timerCallback_ != nullptr && (repeatTimes_ > 0 || repeatTimes_ == RepeatForever))
            {
                timerCallback_();
            }
        }

    private:
        IdType id_ {-1};
        Timestamp expiredTime_ {0};
        ::int32_t repeatTimes_ {0};
        // unit: milliseconds
        TimeType interval_ {0};
        TimerCallback timerCallback_ {};

        // not thread-safe, because there is usually only one TimerManager to manage Timer
        static ::std::atomic<IdType> TimerIdGenerator;

        static constexpr ::int32_t RepeatForever = -1;
    };
} // namespace nets::base

#endif // NETS_BASE_TIMER_H