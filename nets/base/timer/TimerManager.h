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
        using IdType = typename Timer::IdType;
        using TimerCallback = typename Timer::TimerCallback;
        using TimeType = typename Timer::TimeType;
        using TimerContainer = ::std::multimap<Timestamp, Timer>;

    public:
        TimerManager() = default;
        ~TimerManager() = default;

    public:
        template <typename Fn, typename... Args>
        IdType addTimer(const Timestamp& expiredTime, Fn&& fn, Args&&... args);

        template <typename Fn, typename... Args>
        IdType addTimer(const Timestamp& expiredTime, TimeType interval, Fn&& fn, Args&&... args);

        template <typename Fn, typename... Args>
        IdType addTimer(const Timestamp& expiredTime, ::int32_t repeatTimes, TimeType interval, Fn&& fn, Args&&... args);

        template <typename Fn, typename... Args>
        IdType addTimer(Timer* timer, Fn&& fn, Args&&... args);

        void removeTimer(IdType timerId);

    private:
        TimerContainer timers_ {};
    };

    template <typename Fn, typename... Args>
    TimerManager::IdType TimerManager::addTimer(const Timestamp& expiredTime, Fn&& fn, Args&&... args)
    {
        return addTimer(new Timer(expiredTime), ::std::forward<Fn>(fn), ::std::forward<Args>(args)...);
    }

    template <typename Fn, typename... Args>
    TimerManager::IdType TimerManager::addTimer(const Timestamp& expiredTime, TimeType interval, Fn&& fn, Args&&... args)
    {
        return addTimer(new Timer(expiredTime, interval), ::std::forward<Fn>(fn), ::std::forward<Args>(args)...);
    }

    template <typename Fn, typename... Args>
    TimerManager::IdType TimerManager::addTimer(const Timestamp& expiredTime, ::int32_t repeatTimes, TimeType interval,
                                                Fn&& fn, Args&&... args)
    {
        return addTimer(new Timer(expiredTime, repeatTimes, interval), ::std::forward<Fn>(fn),
                        ::std::forward<Args>(args)...);
    }

    template <typename Fn, typename... Args>
    TimerManager::IdType TimerManager::addTimer(Timer* timer, Fn&& fn, Args&&... args)
    {
    }
} // namespace nets::base

#endif // NETS_BASE_TIMER_MANAGER_H