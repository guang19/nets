//
// Created by guang19
//

#include "nets/base/timer/Timer.h"

namespace nets::base
{
    ::std::atomic<Timer::IdType> Timer::TimerIdGenerator = 0;

    Timer::TimerId::TimerId() : key_(-1), value_(0) {}

    Timer::TimerId::TimerId(IdType key, const Timestamp& value) : key_(key), value_(value) {}

    Timer::TimerId::TimerId(const TimerId& other) : key_(other.key_), value_(other.value_) {}

    Timer::TimerId::TimerId(TimerId&& other) noexcept : key_(other.key_), value_(::std::move(other.value_))
    {
        other.key_ = -1;
    }

    Timer::TimerId& Timer::TimerId::operator=(const TimerId& other)
    {
        if (this != &other)
        {
            key_ = other.key_;
            value_ = other.value_;
        }
        return *this;
    }

    Timer::TimerId& Timer::TimerId::operator=(TimerId&& other) noexcept
    {
        if (this != &other)
        {
            key_ = other.key_;
            value_ = ::std::move(other.value_);
            other.key_ = -1;
        }
        return *this;
    }

    Timer::Timer(const Timestamp& expiredTime) : Timer(expiredTime, 1, 0, false) {}

    Timer::Timer(const Timestamp& expiredTime, TimeType interval, bool fixedDelay)
        : Timer(expiredTime, RepeatForever, interval, fixedDelay)
    {
    }

    Timer::Timer(const Timestamp& expiredTime, ::int32_t repeatTimes, TimeType interval, bool fixedDelay)
        : id_(TimerIdGenerator++, expiredTime), repeatTimes_(repeatTimes), interval_(interval), fixedDelay_(fixedDelay)
    {
    }

    Timer::Timer(Timer&& other) noexcept
        : id_(::std::move(other.id_)), repeatTimes_(other.repeatTimes_), interval_(other.interval_), fixedDelay_(other.fixedDelay_),
          timerCallback_(::std::move(other.timerCallback_))
    {
        other.repeatTimes_ = 0;
        other.interval_ = 0;
        other.fixedDelay_ = false;
    }

    Timer& Timer::operator=(Timer&& other) noexcept
    {
        if (this != &other)
        {
            id_ = ::std::move(other.id_);
            repeatTimes_ = other.repeatTimes_;
            interval_ = other.interval_;
            timerCallback_ = ::std::move(other.timerCallback_);
            other.repeatTimes_ = 0;
            other.interval_ = 0;
            other.fixedDelay_ = false;
        }
        return *this;
    }

    void Timer::onTimer(const Timestamp& now)
    {
        if (timerCallback_ != nullptr && isRepeatable())
        {
            timerCallback_();
            if (fixedDelay_)
            {
                id_.value_ = now.plusMilliseconds(interval_);
            }
            else
            {
                id_.value_ = id_.value_.plusMilliseconds(interval_);
            }
            if (repeatTimes_ > 0)
            {
                --repeatTimes_;
            }
        }
    }
} // namespace nets::base