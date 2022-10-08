//
// Created by guang19
//

#include "nets/base/timer/Timer.h"

namespace nets
{
    ::std::atomic<Timer::IdType> Timer::gTimerIdGenerator = 0;

    TimerId::TimerId() : key_(-1), value_(0) {}

    TimerId::TimerId(IdType key, const Timestamp& value) : key_(key), value_(value) {}

    TimerId::TimerId(const TimerId& other) : key_(other.key_), value_(other.value_) {}

    TimerId::TimerId(TimerId&& other) noexcept : key_(other.key_), value_(::std::move(other.value_))
    {
        other.key_ = -1;
    }

    TimerId& TimerId::operator=(const TimerId& other)
    {
        if (this != &other)
        {
            key_ = other.key_;
            value_ = other.value_;
        }
        return *this;
    }

    TimerId& TimerId::operator=(TimerId&& other) noexcept
    {
        if (this != &other)
        {
            key_ = other.key_;
            value_ = ::std::move(other.value_);
            other.key_ = -1;
        }
        return *this;
    }

    bool TimerId::operator==(const TimerId& other) const
    {
        return key_ == other.key_ && value_ == other.value_;
    }

    Timer::Timer(const Timestamp& expiredTime, ::int32_t repeatTimes, TimeType interval, bool fixedDelay)
        : id_(gTimerIdGenerator++, expiredTime), repeatTimes_(repeatTimes), interval_(interval), fixedDelay_(fixedDelay)
    {
    }

    Timer::Timer(Timer&& other) noexcept
        : id_(::std::move(other.id_)), repeatTimes_(other.repeatTimes_), interval_(other.interval_),
          fixedDelay_(other.fixedDelay_), timerCallback_(::std::move(other.timerCallback_))
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
} // namespace nets