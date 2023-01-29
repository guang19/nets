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

#ifndef NETS_EVENT_LOOP_GROUP_H
#define NETS_EVENT_LOOP_GROUP_H

#include "nets/base/ThreadPool.h"
#include "nets/net/core/EventLoop.h"

namespace nets
{
    class EventLoop;
    class EventLoopGroup : Noncopyable
    {
    public:
        using EventLoopRawPtr = EventLoop*;

    private:
        using EventLoopPtr = ::std::unique_ptr<EventLoop>;
        using EventLoopList = ::std::vector<EventLoopPtr>;
        using FutureList = ::std::vector<::std::future<void>>;
        using MutexType = ::std::mutex;
        using LockGuardType = ::std::lock_guard<MutexType>;
        using UniqueLockType = ::std::unique_lock<MutexType>;
        using ConditionVariableType = ::std::condition_variable;

    public:
        explicit EventLoopGroup(Int32Type numOfEventLoops, const StringType& name);
        ~EventLoopGroup() = default;

    public:
        void loopEach();
        void syncEach();
        EventLoopRawPtr next();

        template <typename Fn, typename... Args>
        void execute(Fn&& func, Args&&... args)
        {
            next()->execute(::std::forward<Fn>(func), ::std::forward<Args>(args)...);
        }

        template <typename Fn, typename... Args,
                  typename HasRet = typename ::std::enable_if<
                      !::std::is_void<typename ::std::invoke_result<Fn&&, Args&&...>::type>::value>::type>
        ::std::future<typename ::std::invoke_result<Fn&&, Args&&...>::type> submit(Fn&& func, Args&&... args)
        {
            return next()->submit(::std::forward<Fn>(func), ::std::forward<Args>(args)...);
        }

        template <typename Fn, typename... Args,
                  typename HasRet = typename ::std::enable_if<
                      ::std::is_void<typename ::std::invoke_result<Fn&&, Args&&...>::type>::value>::type>
        ::std::future<void> submit(Fn&& func, Args&&... args)
        {
            return next()->submit(::std::forward<Fn>(func), ::std::forward<Args>(args)...);
        }

        void shutdown();
        bool isShutdown() const;

    private:
        SizeType nextLoop_;
        Int32Type numOfEventLoops_;
        EventLoopList eventLoops_;
        FutureList futures_;
        ThreadPool eventLoopThreadPool_;
        MutexType mutex_;
        ConditionVariableType cv_;
    };
} // namespace nets

#endif // NETS_EVENT_LOOP_GROUP_H