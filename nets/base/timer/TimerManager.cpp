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

// @brief Multimap based min-heap timer manager implementation

#include "nets/base/timer/TimerManager.h"

namespace nets
{
    void TimerManager::removeTimer(const TimerId& timerId)
    {
        // The timer to be deleted may have already been executed, and it's time may have been incremented, so start
        // searching from lower
        auto lower = timers_.lower_bound(timerId.value_);
        for (auto it = lower; it != timers_.end(); ++it)
        {
            if (timerId.key_ == it->second.id().key_)
            {
                // remove timer in TimerManager::update
                it->second.setExpiredTime(Timestamp());
                it->second.setRepeatTimes(0);
                return;
            }
        }
    }

    void TimerManager::update()
    {
        if (timers_.empty())
        {
            return;
        }
        for (auto it = timers_.begin(); it != timers_.end();)
        {
            Timestamp now(Timestamp::now());
            // if current timer is not expired, following timers do not need to e checked
            if (!it->second.isExpired(now))
            {
                return;
            }
            // trigger
            it->second.onTimer(now);
            // if timer can still trigger next time
            if (it->second.isRepeatable())
            {
                // erase it and reinsert
                Timer newTimer(::std::move(it->second));
                auto nextIt = timers_.erase(it);
                auto newIt = timers_.insert(::std::make_pair(newTimer.expiredTime(), ::std::move(newTimer)));
                if (nextIt != timers_.end() && nextIt->first < newIt->first)
                {
                    it = nextIt;
                }
                else
                {
                    // if next timer expired time more newExpiredTime or current timer is last one of timers
                    it = newIt;
                }
            }
            else
            {
                it = timers_.erase(it);
            }
        }
    }

    TimeType TimerManager::nearestTimerRemainingExpiredTime()
    {
        auto begin = timers_.begin();
        if (begin == timers_.end())
        {
            return -1;
        }
        Timestamp now(Timestamp::now());
        if (begin->first <= now)
        {
            return 0;
        }
        else
        {
            return begin->first.minusMicroseconds(now.microsecondsSinceEpoch()).millisecondsSinceEpoch();
        }
    }
} // namespace nets