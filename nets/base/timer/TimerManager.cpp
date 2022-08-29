//
// Created by guang19
//

#include "nets/base/timer/TimerManager.h"

namespace nets::base
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
} // namespace nets::base