//
// Created by guang19
//

#ifndef NETS_EVENT_LOOP_GROUP_H
#define NETS_EVENT_LOOP_GROUP_H

#include "nets/base/concurrency/ThreadPool.h"
#include "nets/net/core/EventLoop.h"

namespace nets
{
    class EventLoopGroup : Noncopyable
    {
    public:
        using IntType = typename ThreadPool::IntType;
        using StringType = ::std::string;
        using EventLoopRawPtr = EventLoop*;
        using EventLoopPtr = ::std::unique_ptr<EventLoop>;
        using EventLoopList = ::std::vector<EventLoopPtr>;
        using FutureList = ::std::vector<::std::future<void>>;
        using SizeType = typename EventLoopList::size_type;
        using MutexType = ::std::mutex;
        using LockGuardType = ::std::lock_guard<MutexType>;
        using UniqueLockType = ::std::unique_lock<MutexType>;
        using ConditionVariableType = ::std::condition_variable;

    public:
        explicit EventLoopGroup(IntType numOfEventLoops, const StringType& name);
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
        IntType numOfEventLoops_;
        EventLoopList eventLoops_;
        FutureList futures_;
        ThreadPool eventLoopThreadPool_;
        MutexType mutex_;
        ConditionVariableType cv_;
    };
} // namespace nets

#endif // NETS_EVENT_LOOP_GROUP_H