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

// @brief The EventLoopGroup manages all EventLoops

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