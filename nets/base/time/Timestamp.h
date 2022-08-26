//
// Created by guang19
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
        explicit Timestamp(TimeType secondsSinceEpoch);
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

    public:
        static Timestamp now();

        inline TimeType seconds() const
        {
            return static_cast<TimeType>(timestampSinceEpoch_ / MicrosecondsPerSecond);
        }

        inline TimeType milliseconds() const
        {
            return static_cast<TimeType>(timestampSinceEpoch_ / MillisecondsPerSecond);
        }

        inline TimeType microseconds() const
        {
            return timestampSinceEpoch_;
        }

        inline TimeType microsPartOfTimestamp() const
        {
            return static_cast<TimeType>(timestampSinceEpoch_ % MicrosecondsPerSecond);
        }

    private:
        TimeType timestampSinceEpoch_ {0};
    };
} // namespace nets::base

#endif // NETS_BASE_TIMESTAMP_H