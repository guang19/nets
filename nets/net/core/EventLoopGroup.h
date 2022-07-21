//
// Created by guang19 on 2022/5/25.
//

#ifndef NETS_NET_EVENT_LOOP_GROUP_H
#define NETS_NET_EVENT_LOOP_GROUP_H

#include "nets/base/concurrency/ThreadPool.h"
#include "nets/net/core/EventLoop.h"

namespace nets::net
{
    class EventLoopGroup : nets::base::Noncopyable
    {
    public:
        using NType = typename nets::base::ThreadPool::NType;
        using ThreadPoolType = nets::base::ThreadPool;
        using ThreadPoolPtr = ::std::unique_ptr<ThreadPoolType>;
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
        explicit EventLoopGroup(NType numOfEventLoops, const ::std::string& name);
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

    private:
        NType nextLoop_ {0};
        NType numOfEventLoops_ {0};
        EventLoopList eventLoops_ {};
        FutureList futures_ {};
        ThreadPoolPtr eventLoopThreadPool_ {nullptr};
        MutexType mutex_ {};
        ConditionVariableType cv_ {};
    };
} // namespace nets::net

#endif // NETS_NET_EVENT_LOOP_GROUP_H
