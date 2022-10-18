// MIT License
//
// Copyright (c) 2022 guang19
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

// @brief Multimap based min-heap timer manager implementation

#ifndef NETS_TIMER_MANAGER_H
#define NETS_TIMER_MANAGER_H

#include <map>

#include "nets/base/timer/Timer.h"

namespace nets
{
    class TimerManager : Noncopyable
    {
    public:
        TimerManager() = default;
        ~TimerManager() = default;

    public:
        // time unit: ms
        template <typename Fn, typename... Args>
        TimerId addTimer(const Timestamp& expiredTime, Int32Type repeatTimes, TimeType interval, bool fixedDelay, Fn&& fn,
                         Args&&... args);

        void removeTimer(const TimerId& timerId);
        void update();

        TimeType nearestTimerRemainingExpiredTime();

    private:
        using TimerContainer = ::std::multimap<Timestamp, Timer>;
        TimerContainer timers_ {};
    };

    template <typename Fn, typename... Args>
    TimerId TimerManager::addTimer(const Timestamp& expiredTime, Int32Type repeatTimes, TimeType interval, bool fixedDelay,
                                   Fn&& fn, Args&&... args)
    {
        Timer timer(expiredTime, repeatTimes, interval, fixedDelay);
        TimerId timerId = timer.id();
        timer.setTimerCallback(::std::forward<Fn>(fn), ::std::forward<Args>(args)...);
        timers_.insert(::std::make_pair(expiredTime, ::std::move(timer)));
        return timerId;
    }
} // namespace nets

#endif // NETS_TIMER_MANAGER_H