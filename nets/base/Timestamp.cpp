//
// Created by guang19
//

#include "nets/base/Timestamp.h"

#include <sys/time.h>
#include <utility>

namespace nets::base
{
    Timestamp::Timestamp() : Timestamp(0) {}

    Timestamp::Timestamp(TimeType microsecondsSinceEpoch) : microsecondsSinceEpoch_(microsecondsSinceEpoch) {}

    Timestamp::Timestamp(const TimeVal& tmv) : Timestamp(tmv.tv_sec, tmv.tv_usec) {}

    Timestamp::Timestamp(TimeType secondsSinceEpoch, TimeType microseconds)
        : microsecondsSinceEpoch_((secondsSinceEpoch * MicrosecondsPerSecond) + microseconds)
    {
    }

    Timestamp::Timestamp(const Timestamp& other) : microsecondsSinceEpoch_(other.microsecondsSinceEpoch_) {}

    Timestamp::Timestamp(Timestamp&& other) noexcept : microsecondsSinceEpoch_(other.microsecondsSinceEpoch_)
    {
        other.microsecondsSinceEpoch_ = 0;
    }

    Timestamp& Timestamp::operator=(const Timestamp& other)
    {
        if (this != &other)
        {
            microsecondsSinceEpoch_ = other.microsecondsSinceEpoch_;
        }
        return *this;
    }

    Timestamp& Timestamp::operator=(Timestamp&& other) noexcept
    {
        if (this != &other)
        {
            microsecondsSinceEpoch_ = other.microsecondsSinceEpoch_;
            other.microsecondsSinceEpoch_ = 0;
        }
        return *this;
    }

    void Timestamp::swap(Timestamp&& other)
    {
        ::std::swap(microsecondsSinceEpoch_, other.microsecondsSinceEpoch_);
    }

    Timestamp Timestamp::now()
    {
        TimeVal tmv {};
        ::gettimeofday(&tmv, nullptr);
        return Timestamp(tmv);
    }
} // namespace nets::base
