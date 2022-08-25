//
// Created by YangGuang on 2022/5/2.
//

#ifndef NETS_BASE_TIMESTAMP_H
#define NETS_BASE_TIMESTAMP_H

#include <cstdint>
#include <ctime>

#include "nets/base/Copyable.h"

namespace nets::base
{
    namespace
    {
        constexpr ::int32_t MillisecondsPerSecond = 1000;
        constexpr ::int32_t MicrosecondsPerSecond = 1000000;
    } // namespace

    class Timestamp : public Copyable
    {
    public:
        using TimeType = ::time_t;

    public:
        Timestamp();
        explicit Timestamp(TimeType timestamp);
        explicit Timestamp(TimeType secondsSinceEpoch, ::int32_t microseconds);
        ~Timestamp() = default;

        Timestamp(const Timestamp& other);
        Timestamp(Timestamp&& other) noexcept;
        Timestamp& operator=(const Timestamp& other);
        Timestamp& operator=(Timestamp&& other) noexcept;

        void swap(Timestamp&& other);

    public:
        inline bool operator<(const Timestamp& other) const
        {
            return timestampSinceEpoch_ < other.timestampSinceEpoch_;
        }

        inline bool operator>(const Timestamp& other) const
        {
            return timestampSinceEpoch_ > other.timestampSinceEpoch_;
        }

        inline bool operator==(const Timestamp& other) const
        {
            return timestampSinceEpoch_ == other.timestampSinceEpoch_;
        }

        inline bool operator<=(const Timestamp& other) const
        {
            return timestampSinceEpoch_ <= other.timestampSinceEpoch_;
        }

        inline bool operator>=(const Timestamp& other) const
        {
            return timestampSinceEpoch_ >= other.timestampSinceEpoch_;
        }

        inline Timestamp operator-(const Timestamp& other) const
        {
            return Timestamp(timestampSinceEpoch_ - other.timestampSinceEpoch_);
        }

        inline Timestamp operator+(const Timestamp& other) const
        {
            return Timestamp(timestampSinceEpoch_ + other.timestampSinceEpoch_);
        }

    public:
        static Timestamp now();

        inline TimeType timestamp() const
        {
            return timestampSinceEpoch_;
        }

        inline TimeType secsFromTimestamp() const
        {
            return static_cast<TimeType>(timestampSinceEpoch_ / MicrosecondsPerSecond);
        }

        inline ::int32_t microsFromTimestamp() const
        {
            return static_cast<::int32_t>(timestampSinceEpoch_ % MicrosecondsPerSecond);
        }

    private:
        TimeType timestampSinceEpoch_ {0};
    };
} // namespace nets::base

#endif // NETS_BASE_TIMESTAMP_H