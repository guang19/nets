//
// Created by guang19
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
        using IntType = typename nets::base::ThreadPool::IntType;
        using StringType = ::std::string;
        using ThreadPoolType = typename nets::base::ThreadPool;
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
        SizeType nextLoop_ {0};
        IntType numOfEventLoops_ {0};
        EventLoopList eventLoops_ {};
        FutureList futures_ {};
        ThreadPoolType eventLoopThreadPool_;
        MutexType mutex_ {};
        ConditionVariableType cv_ {};
    };
} // namespace nets::net

#endif // NETS_NET_EVENT_LOOP_GROUP_H