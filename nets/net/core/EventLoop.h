//
// Created by guang19 on 2022/5/15.
//

#ifndef NETS_NET_EVENT_LOOP_H
#define NETS_NET_EVENT_LOOP_H

#include <atomic>
#include <cassert>
#include <functional>
#include <future>
#include <memory>
#include <vector>

#include "nets/base/log/Logging.h"
#include "nets/base/Noncopyable.h"
#include "nets/base/ThreadHelper.h"
#include "nets/net/core/NotifyChannel.h"

namespace nets::net
{
	class Channel;
	class Poller;
	class PollerFactory;
	class EventLoop;

	class EventLoop : nets::base::Noncopyable
	{
	public:
		using TaskType = ::std::function<void()>;
		using TaskList = ::std::vector<TaskType>;
		using MutexType = ::std::mutex;
		using LockGuardType = ::std::lock_guard<MutexType>;
		using ChannelPtr = ::std::shared_ptr<Channel>;
		using ChannelList = ::std::vector<ChannelPtr>;
		using NotifyChannelPtr = ::std::shared_ptr<NotifyChannel>;
		using PollerPtr = ::std::unique_ptr<Poller>;
		using EventLoopRawPtr = EventLoop*;

	public:
		EventLoop();
		~EventLoop();

	public:
		void run();
		void shutdown();

		bool inCurrentEventLoop() const;
		EventLoopRawPtr currentEventLoop() const;

		void registerChannel(ChannelPtr channel);
		void modifyChannel(ChannelPtr channel);
		void deregisterChannel(ChannelPtr channel);
		bool hasChannel(ChannelPtr channel);

		template <typename Fn, typename... Args>
		void execute(Fn&& func, Args&&... args);

		template <typename Fn, typename... Args,
				  typename HasRet = typename ::std::enable_if<
					  !::std::is_void<typename ::std::invoke_result<Fn&&, Args&&...>::type>::value>::type>
		::std::future<typename ::std::invoke_result<Fn&&, Args&&...>::type> submit(Fn&& func, Args&&... args);

		template <typename Fn, typename... Args,
				  typename HasRet = typename ::std::enable_if<
					  ::std::is_void<typename ::std::invoke_result<Fn&&, Args&&...>::type>::value>::type>
		::std::future<void> submit(Fn&& func, Args&&... args);

	private:
		void runPendingTasks();

	private:
		::std::atomic_bool running_ {false};
		const ::pid_t threadId_ {0};
		NotifyChannelPtr notifier_ {nullptr};
		ChannelList activeChannels_ {nullptr};
		PollerPtr poller_ {nullptr};
		TaskList pendingTasks_ {};
		MutexType mutex_ {};
	};

	template <typename Fn, typename... Args>
	void EventLoop::execute(Fn&& func, Args&&... args)
	{
		TaskType task = ::std::bind(::std::forward<Fn>(func), ::std::forward<Args>(args)...);
		if (inCurrentEventLoop())
		{
			task();
		}
		else
		{
			{
				LockGuardType lock(mutex_);
				pendingTasks_.push_back(::std::move(task));
			}
			notifier_->notify();
		}
	}

	template <typename Fn, typename... Args, typename HasRet>
	::std::future<typename ::std::invoke_result<Fn&&, Args&&...>::type> EventLoop::submit(Fn&& func, Args&&... args)
	{
		using RetType = typename ::std::invoke_result<Fn&&, Args&&...>::type;
		auto promise = ::std::make_shared<::std::promise<RetType>>();
		auto future = promise->get_future();
		::std::function<RetType()> task = ::std::bind(::std::forward<Fn>(func), ::std::forward<Args>(args)...);
		TaskType promiseTask = [this, promise, f = ::std::move(task)]() mutable
		{
			assert(promise.use_count() > 0);
			try
			{
				RetType result = f();
				promise->set_value(result);
			}
			catch (const ::std::exception& exception)
			{
				promise->set_exception(::std::make_exception_ptr(exception));
				LOGS_ERROR << "EventLoop::promiseTask exception caught during thread [" << nets::base::currentTid()
						   << "] execution in event loop thread [" << threadId_ << "], reason " << exception.what();
			}
			catch (...)
			{
				promise->set_exception(::std::current_exception());
				LOGS_ERROR << "EventLoop::promiseTask exception caught during thread [" << nets::base::currentTid()
						   << "] execution in event loop thread [" << threadId_ << "]";
			}
		};
		assert(2 == promise.use_count());
		execute(::std::move(promiseTask));
		return future;
	}

	template <typename Fn, typename... Args, typename HasRet>
	::std::future<void> EventLoop::submit(Fn&& func, Args&&... args)
	{
		auto promise = ::std::make_shared<::std::promise<void>>();
		auto future = promise->get_future();
		::std::function<void()> task = ::std::bind(::std::forward<Fn>(func), ::std::forward<Args>(args)...);
		TaskType promiseTask = [this, promise, f = ::std::move(task)]() mutable
		{
			assert(promise.use_count() > 0);
			try
			{
				f();
				promise->set_value();
			}
			catch (const ::std::exception& exception)
			{
				promise->set_exception(::std::make_exception_ptr(exception));
				LOGS_ERROR << "EventLoop::promiseTask exception caught during thread [" << nets::base::currentTid()
						   << "] execution in event loop thread [" << threadId_ << "], reason " << exception.what();
			}
			catch (...)
			{
				promise->set_exception(::std::current_exception());
				LOGS_ERROR << "EventLoop::promiseTask exception caught during thread [" << nets::base::currentTid()
						   << "] execution in event loop thread [" << threadId_ << "]";
			}
		};
		assert(2 == promise.use_count());
		execute(::std::move(promiseTask));
		return future;
	}
} // namespace nets::net

#endif // NETS_NET_EVENT_LOOP_H
