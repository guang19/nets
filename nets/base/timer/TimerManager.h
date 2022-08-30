//
// Created by guang19
//

#ifndef NETS_BASE_TIMER_MANAGER_H
#define NETS_BASE_TIMER_MANAGER_H

#include <map>

#include "nets/base/timer/Timer.h"

namespace nets::base
{

    class TimerManager : Noncopyable
    {
    public:
        using TimerId = typename Timer::TimerId;
        using TimeType = typename Timer::TimeType;
        using TimerContainer = ::std::multimap<Timestamp, Timer>;

    public:
        TimerManager() = default;
        ~TimerManager() = default;

    public:
        // time unit: ms
        template <typename Fn, typename... Args>
        TimerId addTimer(const Timestamp& expiredTime, ::int32_t repeatTimes, TimeType interval, bool fixedDelay, Fn&& fn,
                         Args&&... args);

        void removeTimer(const TimerId& timerId);
        void update();

        TimeType nearestTimerRemainingExpiredTime();

    private:
        TimerContainer timers_ {};
    };

    template <typename Fn, typename... Args>
    TimerManager::TimerId TimerManager::addTimer(const Timestamp& expiredTime, ::int32_t repeatTimes, TimeType interval,
                                                 bool fixedDelay, Fn&& fn, Args&&... args)
    {
        Timer timer(expiredTime, repeatTimes, interval, fixedDelay);
        TimerId timerId = timer.id();
        timer.setTimerCallback(::std::forward<Fn>(fn), ::std::forward<Args>(args)...);
        timers_.insert(::std::make_pair(expiredTime, ::std::move(timer)));
        return timerId;
    }
} // namespace nets::base

#endif // NETS_BASE_TIMER_MANAGER_H