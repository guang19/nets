//
// Created by guang19 on 2022/5/25.
//

#include "nets/net/core/EventLoopGroup.h"

namespace nets::net
{
    EventLoopGroup::EventLoopGroup(IntType numOfEventLoops, const StringType& name)
        : nextLoop_(0), numOfEventLoops_(numOfEventLoops), mutex_(), cv_()
    {
        if (numOfEventLoops_ <= 0)
        {
            THROW_FMT(::std::invalid_argument, "numOfEventLoops must be greater than 0,numOfEventLoops=%u", numOfEventLoops_);
        }
        eventLoops_.reserve(numOfEventLoops_);
        futures_.reserve(numOfEventLoops);
        eventLoopThreadPool_ = ::std::make_unique<ThreadPoolType>(numOfEventLoops_, numOfEventLoops_, 0, name);
    }

    void EventLoopGroup::loopEach()
    {
        for (IntType i = 0; i < numOfEventLoops_; ++i)
        {
            futures_[i] = eventLoopThreadPool_->submit(
                [this]()
                {
                    auto eventLoop = new EventLoop();
                    {
                        LockGuardType lock(mutex_);
                        eventLoops_.push_back(::std::unique_ptr<EventLoop>(eventLoop));
                        if (eventLoops_.size() == numOfEventLoops_)
                        {
                            cv_.notify_one();
                        }
                    }
                    eventLoop->run();
                });
        }
        UniqueLockType lock(mutex_);
        cv_.wait(lock,
                 [this]() -> bool
                 {
                     return eventLoops_.size() == numOfEventLoops_;
                 });
    }

    void EventLoopGroup::syncEach()
    {
        for (IntType i = 0; i < numOfEventLoops_; ++i)
        {
            futures_[i].wait();
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
} // namespace nets::net