//
// Created by guang19
//

#include "nets/net/core/EventLoopGroup.h"

namespace nets
{
    EventLoopGroup::EventLoopGroup(IntType numOfEventLoops, const StringType& name)
        : nextLoop_(0), numOfEventLoops_(numOfEventLoops), eventLoops_(), futures_(),
          eventLoopThreadPool_(numOfEventLoops_, numOfEventLoops_, 0, name), mutex_(), cv_()
    {
        eventLoops_.reserve(numOfEventLoops_);
        futures_.reserve(numOfEventLoops_);
    }

    void EventLoopGroup::loopEach()
    {
        for (IntType i = 0; i < numOfEventLoops_; ++i)
        {
            futures_.emplace_back(eventLoopThreadPool_.submit(
                [this]()
                {
                    auto eventLoop = new EventLoop();
                    {
                        LockGuardType lock(mutex_);
                        eventLoops_.push_back(::std::unique_ptr<EventLoop>(eventLoop));
                        if (eventLoops_.size() == static_cast<SizeType>(numOfEventLoops_))
                        {
                            cv_.notify_one();
                        }
                    }
                    eventLoop->run();
                }));
        }
        UniqueLockType lock(mutex_);
        cv_.wait(lock,
                 [this]() -> bool
                 {
                     return eventLoops_.size() == static_cast<SizeType>(numOfEventLoops_);
                 });
    }

    void EventLoopGroup::syncEach()
    {
        for (const auto& future : futures_)
        {
            future.wait();
        }
    }

    EventLoopGroup::EventLoopRawPtr EventLoopGroup::next()
    {
        EventLoopRawPtr nextEventLoop = eventLoops_[nextLoop_++].get();
        if (nextLoop_ >= eventLoops_.size())
        {
            nextLoop_ = 0;
        }
        return nextEventLoop;
    }

    void EventLoopGroup::shutdown()
    {
        for (auto& eventLoop : eventLoops_)
        {
            if (eventLoop->isRunning())
            {
                eventLoop->shutdown();
            }
        }
    }

    bool EventLoopGroup::isShutdown() const
    {
        for (const auto& eventLoop : eventLoops_)
        {
            if (eventLoop->isRunning())
            {
                return false;
            }
        }
        return true;
    }
} // namespace nets