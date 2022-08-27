//
// Created by guang19
//

#include "nets/base/timer/Timer.h"

namespace nets::base
{
    ::std::atomic<Timer::IdType> Timer::TimerIdGenerator = 0;

    Timer::Timer() : Timer(Timestamp(0)) {}

    Timer::Timer(const Timestamp& expiredTime) : Timer(expiredTime, 0, 0) {}

    Timer::Timer(const Timestamp& expiredTime, TimeType interval) : Timer(expiredTime, RepeatForever, interval) {}

    Timer::Timer(const Timestamp& expiredTime, ::int32_t repeatTimes, TimeType interval)
        : id_(TimerIdGenerator++), expiredTime_(expiredTime), repeatTimes_(repeatTimes), interval_(interval)
    {
    }

    Timer::Timer(Timer&& other) noexcept
    {
        id_ = other.id_;
        expiredTime_ = other.expiredTime_;
        repeatTimes_ = other.repeatTimes_;
        interval_ = other.interval_;
        timerCallback_ = ::std::move(other.timerCallback_);
        other.id_ = -1;
        other.expiredTime_ = Timestamp();
        other.repeatTimes_ = 0;
        other.interval_ = 0;
    }

    Timer& Timer::operator=(Timer&& other) noexcept
    {
        if (this != &other)
        {
            id_ = other.id_;
            expiredTime_ = other.expiredTime_;
            repeatTimes_ = other.repeatTimes_;
            interval_ = other.interval_;
            timerCallback_ = ::std::move(other.timerCallback_);
            other.id_ = -1;
            other.expiredTime_ = Timestamp();
            other.repeatTimes_ = 0;
            other.interval_ = 0;
        }
        return *this;
    }
} // namespace nets::base