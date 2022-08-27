//
// Created by guang19
//

#include "nets/base/timer/Timer.h"

namespace nets::base
{
    ::std::atomic<Timer::IdType> Timer::TimerIdGenerator = 0;

    Timer::Timer() : Timer(0) {}

    Timer::Timer(TimeType expiredTime) : Timer(expiredTime, false, 0) {}

    Timer::Timer(TimeType expiredTime, ::int32_t repeatTimes, TimeType interval)
        : id_(TimerIdGenerator++), expiredTime_(expiredTime), repeatTimes_(repeatTimes), interval_(interval)
    {
    }
} // namespace nets::base