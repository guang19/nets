//
// Created by guang19
//

#include "nets/base/time/Timestamp.h"

#include <sys/time.h>
#include <utility>

namespace nets::base
{
    Timestamp::Timestamp() : Timestamp(0) {}

    Timestamp::Timestamp(TimeType secondsSinceEpoch) : Timestamp(secondsSinceEpoch, 0) {}

    Timestamp::Timestamp(TimeType secondsSinceEpoch, TimeType microseconds)
        : timestampSinceEpoch_((secondsSinceEpoch * MicrosecondsPerSecond) + microseconds)
    {
    }

    Timestamp::Timestamp(const Timestamp& other)
    {
        timestampSinceEpoch_ = other.timestampSinceEpoch_;
    }

    Timestamp::Timestamp(Timestamp&& other) noexcept
    {
        timestampSinceEpoch_ = other.timestampSinceEpoch_;
        other.timestampSinceEpoch_ = 0;
    }

    Timestamp& Timestamp::operator=(const Timestamp& other)
    {
        if (this != &other)
        {
            timestampSinceEpoch_ = other.timestampSinceEpoch_;
        }
        return *this;
    }

    Timestamp& Timestamp::operator=(Timestamp&& other) noexcept
    {
        if (this != &other)
        {
            timestampSinceEpoch_ = other.timestampSinceEpoch_;
            other.timestampSinceEpoch_ = 0;
        }
        return *this;
    }

    void Timestamp::swap(Timestamp&& other)
    {
        ::std::swap(timestampSinceEpoch_, other.timestampSinceEpoch_);
    }

    Timestamp Timestamp::now()
    {
        using TimeVal = struct timeval;
        TimeVal tmV {};
        ::gettimeofday(&tmV, nullptr);
        return Timestamp(tmV.tv_sec, tmV.tv_usec);
    }
} // namespace nets::base
