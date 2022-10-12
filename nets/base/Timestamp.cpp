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

#include "nets/base/Timestamp.h"

#include <sys/time.h>
#include <utility>

namespace nets
{
    Timestamp::Timestamp() : Timestamp(0) {}

    Timestamp::Timestamp(TimeType microsecondsSinceEpoch) : microsecondsSinceEpoch_(microsecondsSinceEpoch) {}

    Timestamp::Timestamp(const TimeVal& tmv) : Timestamp(tmv.tv_sec, tmv.tv_usec) {}

    Timestamp::Timestamp(TimeType secondsSinceEpoch, TimeType microseconds)
        : microsecondsSinceEpoch_((secondsSinceEpoch * kMicrosecondsPerSecond) + microseconds)
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
} // namespace nets
