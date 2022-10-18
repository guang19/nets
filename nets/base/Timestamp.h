// MIT License
//
// Copyright (c) 2022 guang19
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

// @brief The encapsulation of timestamp operations

#ifndef NETS_TIMESTAMP_H
#define NETS_TIMESTAMP_H

#include <cstdint>
#include <ctime>

#include "nets/base/Copyable.h"
#include "nets/base/Types.h"

namespace nets
{
    namespace
    {
        constexpr Int32Type kMillisecondsPerSecond = 1000;
        constexpr Int32Type kMicrosecondsPerMillisecond = 1000;
        constexpr Int32Type kMicrosecondsPerSecond = 1000000;
    } // namespace

    class Timestamp : public Copyable
    {
    public:
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