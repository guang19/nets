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
        using TimerId = Timer::TimerId;
        using TimerCallback = typename Timer::TimerCallback;
        using TimeType = typename Timer::TimeType;
        using TimerContainer = ::std::multimap<Timestamp, Timer>;

    public:
        TimerManager() = default;
        ~TimerManager() = default;

    public:
        template <typename Fn, typename... Args>
        TimerId addNonRepeatableTimer(const Timestamp& expiredTime, Fn&& fn, Args&&... args);

        template <typename Fn, typename... Args>
        TimerId addFixedRateTimer(const Timestamp& expiredTime, TimeType interval, Fn&& fn, Args&&... args);

        template <typename Fn, typename... Args>
        TimerId addFixedDelayTimer(const Timestamp& expiredTime, TimeType interval, Fn&& fn, Args&&... args);

        template <typename Fn, typename... Args>
        TimerId addTimer(const Timestamp& expiredTime, ::int32_t repeatTimes, TimeType interval, bool fixedDelay, Fn&& fn,
                         Args&&... args);

        void removeTimer(const TimerId& timerId);
        void update();

    private:
        TimerContainer timers_ {};
    };

    template <typename Fn, typename... Args>
    TimerManager::TimerId TimerManager::addNonRepeatableTimer(const Timestamp& expiredTime, Fn&& fn, Args&&... args)
    {
        return addTimer(expiredTime, 1, 0, false, ::std::forward<Fn>(fn), ::std::forward<Args>(args)...);
    }

    template <typename Fn, typename... Args>
    TimerManager::TimerId TimerManager::addFixedRateTimer(const Timestamp& expiredTime, TimeType interval, Fn&& fn,
                                                          Args&&... args)
    {
        return addTimer(expiredTime, Timer::RepeatForever, interval, false, ::std::forward<Fn>(fn),
                        ::std::forward<Args>(args)...);
    }

    template <typename Fn, typename... Args>
    TimerManager::TimerId TimerManager::addFixedDelayTimer(const Timestamp& expiredTime, TimeType interval, Fn&& fn,
                                                           Args&&... args)
    {
        return addTimer(expiredTime, Timer::RepeatForever, interval, true, ::std::forward<Fn>(fn),
                        ::std::forward<Args>(args)...);
    }

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