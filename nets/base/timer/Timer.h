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

// @brief Implementation of a timer

#ifndef NETS_TIMER_H
#define NETS_TIMER_H

#include <atomic>
#include <functional>

#include "nets/base/Noncopyable.h"
#include "nets/base/Timestamp.h"

namespace nets
{
    struct TimerId : public Copyable
    {
    public:
        using IdType = ::int64_t;

    public:
        TimerId();
        explicit TimerId(IdType key, const Timestamp& value);
        ~TimerId() = default;

        TimerId(const TimerId& other);
        TimerId(TimerId&& other) noexcept;
        TimerId& operator=(const TimerId& other);
        TimerId& operator=(TimerId&& other) noexcept;

        bool operator==(const TimerId& other) const;

        IdType key_;
        Timestamp value_;
    };

    class Timer : Noncopyable
    {
    public:
        using IdType = TimerId::IdType;
        using TimerCallback = ::std::function<void()>;
        static constexpr Int32Type kRepeatForever = -1;

    public:
        explicit Timer(const Timestamp& expiredTime, Int32Type repeatTimes, TimeType interval, bool fixedDelay = false);
        ~Timer() = default;

        // in order to allow map store timer, needs to support move
        Timer(Timer&& other) noexcept;
        Timer& operator=(Timer&& other) noexcept;

        inline const TimerId& id() const
        {
            return id_;
        }

        inline const Timestamp& expiredTime() const
        {
            return id_.value_;
        }

        inline void setExpiredTime(const Timestamp& expiredTime)
        {
            id_.value_ = expiredTime;
        }

        inline bool isRepeatable() const
        {
            return repeatTimes_ == kRepeatForever || repeatTimes_ > 0;
        }

        inline void setRepeatTimes(Int32Type repeatTimes)
        {
            repeatTimes_ = repeatTimes;
        }

        inline bool isExpired(const Timestamp& now) const
        {
            return now >= id_.value_;
        }

        template <typename Fn, typename... Args>
        inline void setTimerCallback(Fn&& fn, Args&&... args)
        {
            timerCallback_ = ::std::bind(::std::forward<Fn>(fn), ::std::forward<Args>(args)...);
        }

        void onTimer(const Timestamp& now);

    private:
        TimerId id_;
        Int32Type repeatTimes_;
        // unit: milliseconds
        TimeType interval_;
        bool fixedDelay_;
        TimerCallback timerCallback_;

        // not thread-safe,but there is usually only one TimerManager to manage Timer
        static ::std::atomic<IdType> timerIdGenerator_;
    };
} // namespace nets

#endif // NETS_TIMER_H