//
// Created by guang19
//

#include "nets/base/timer/Timer.h"

namespace nets::base
{
    ::std::atomic<Timer::IdType> Timer::TimerIdGenerator = 0;

    Timer::Timer() : Timer(0) {}

    Timer::Timer(TimeType expiredTime) : Timer(expiredTime, false, 0) {}

    Timer::Timer(TimeType expiredTime, bool repeat, TimeType interval)
        : id_(TimerIdGenerator++), expiredTime_(expiredTime), repeat_(repeat), interval_(interval)
    {
    }
} // namespace nets::base