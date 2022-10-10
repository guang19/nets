//
// Created by guang19
//

#ifndef NETS_TIMESTAMP_H
#define NETS_TIMESTAMP_H

#include <cstdint>
#include <ctime>

#include "nets/base/Copyable.h"

namespace nets
{
    namespace
    {
        constexpr ::int32_t kMillisecondsPerSecond = 1000;
        constexpr ::int32_t kMicrosecondsPerMillisecond = 1000;
        constexpr ::int32_t kMicrosecondsPerSecond = 1000000;
    } // namespace

    class Timestamp : public Copyable
    {
    public:
        using TimeType = ::time_t;
        using TimeVal = struct timeval;

    public:
        Timestamp();
        explicit Timestamp(TimeType microsecondsSinceEpoch);
        explicit Timestamp(const TimeVal& tmv);
        explicit Timestamp(TimeType secondsSinceEpoch, TimeType microseconds);
        ~Timestamp() = default;

        Timestamp(const Timestamp& other);
        Timestamp(Timestamp&& other) noexcept;
        Timestamp& operator=(const Timestamp& other);
        Timestamp& operator=(Timestamp&& other) noexcept;

        void swap(Timestamp&& other);

    public:
        inline bool operator<(const Timestamp& other) const
        {
            return microsecondsSinceEpoch_ < other.microsecondsSinceEpoch_;
        }

        inline bool operator>(const Timestamp& other) const
        {
            return microsecondsSinceEpoch_ > other.microsecondsSinceEpoch_;
        }

        inline bool operator==(const Timestamp& other) const
        {
            return microsecondsSinceEpoch_ == other.microsecondsSinceEpoch_;
        }

        inline bool operator<=(const Timestamp& other) const
        {
            return microsecondsSinceEpoch_ <= other.microsecondsSinceEpoch_;
        }

        inline bool operator>=(const Timestamp& other) const
        {
            return microsecondsSinceEpoch_ >= other.microsecondsSinceEpoch_;
        }

    public:
        static Timestamp now();

        inline Timestamp plusSeconds(TimeType seconds) const
        {
            return Timestamp(microsecondsSinceEpoch_ + (seconds * kMicrosecondsPerSecond));
        }

        inline Timestamp plusMilliseconds(TimeType milliseconds) const
        {
            return Timestamp(microsecondsSinceEpoch_ + (milliseconds * kMicrosecondsPerMillisecond));
        }

        inline Timestamp plusMicroseconds(TimeType microseconds) const
        {
            return Timestamp(microsecondsSinceEpoch_ + microseconds);
        }

        inline Timestamp minusSeconds(TimeType seconds) const
        {
            return Timestamp(microsecondsSinceEpoch_ - (seconds * kMicrosecondsPerSecond));
        }

        inline Timestamp minusMilliseconds(TimeType milliseconds) const
        {
            return Timestamp(microsecondsSinceEpoch_ - (milliseconds * kMicrosecondsPerMillisecond));
        }

        inline Timestamp minusMicroseconds(TimeType microseconds) const
        {
            return Timestamp(microsecondsSinceEpoch_ - microseconds);
        }

        inline TimeType secondsSinceEpoch() const
        {
            return static_cast<TimeType>(microsecondsSinceEpoch_ / kMicrosecondsPerSecond);
        }

        inline TimeType millisecondsSinceEpoch() const
        {
            return static_cast<TimeType>(microsecondsSinceEpoch_ / kMillisecondsPerSecond);
        }

        inline TimeType microsecondsSinceEpoch() const
        {
            return microsecondsSinceEpoch_;
        }

        inline TimeType microsPartOfTimestamp() const
        {
            return static_cast<TimeType>(microsecondsSinceEpoch_ % kMicrosecondsPerSecond);
        }

    private:
        TimeType microsecondsSinceEpoch_;
    };
} // namespace nets

#endif // NETS_TIMESTAMP_H