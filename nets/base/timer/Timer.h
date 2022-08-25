//
// Created by guang19
//

#ifndef NETS_BASE_TIMER_H
#define NETS_BASE_TIMER_H

#include <atomic>
#include <cstdint>
#include <functional>

#include "nets/base/Noncopyable.h"

namespace nets::base
{
    class Timer : Noncopyable
    {
    public:
        using TimeType = ::time_t;
        using IdType = int64_t;
        using TimerCallback = ::std::function<void()>;

    public:
        Timer();
        explicit Timer(TimeType expiredTime);
        explicit Timer(TimeType expiredTime, bool repeat, TimeType interval);

        ~Timer() = default;

        IdType id() const
        {
            return id_;
        }

        TimeType expiredTime() const
        {
            return expiredTime_;
        }

        bool isRepeat() const
        {
            return repeat_;
        }

        template<typename Fn, typename ...Args>
        void setTimerCallback(Fn&& fn, Args&& ...args)
        {
            timerCallback_ = ::std::bind(::std::forward<Fn>(fn), ::std::forward<Args>(args)...);
        }

        void onTimer()
        {
            if (timerCallback_ != nullptr)
            {
                timerCallback_();
            }
        }

    private:
        IdType id_ {-1};
        TimeType expiredTime_ {0};
        bool repeat_ {false};
        TimeType interval_ {0};
        TimerCallback timerCallback_ {};

        // not thread-safe, because there is usually only one TimerManager to manage Timer
        static ::std::atomic<IdType> TimerIdGenerator;
    };
} // namespace nets::base

#endif // NETS_BASE_TIMER_H
