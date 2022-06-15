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

namespace nets::net
{
	class Channel;
	class Poller;
	class PollerFactory;
	class EventLoop;

	class EventLoop : nets::base::Noncopyable
	{
	public:
		using FunctorType = ::std::function<void()>;
		using FunctorList = ::std::vector<FunctorType>;
		using MutexType = ::std::mutex;
		using LockGuardType = ::std::lock_guard<MutexType>;
		using ChannelPtr = ::std::shared_ptr<Channel>;
		using ChannelList = ::std::shared_ptr<::std::vector<ChannelPtr>>;
		using PollerPtr = ::std::unique_ptr<Poller>;
		using EventLoopRawPtr = EventLoop*;

	public:
		EventLoop();
		~EventLoop();

	public:
		void loop();
		void shutdown();

		bool isInCurrentEventLoop() const;
		EventLoopRawPtr currentThreadEventLoop() const;

		void notify();

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
		template <typename RetType>
		FunctorType makeTask(const ::std::shared_ptr<::std::promise<RetType>>& promise,
							 ::std::function<RetType()> promiseTask);

		FunctorType makeTask(const ::std::shared_ptr<::std::promise<void>>& promise, ::std::function<void()> promiseTask);

	private:
		::std::atomic_bool running_ {false};
		const ::pid_t threadId_ {0};
		PollerPtr poller_ {nullptr};
		ChannelList activeChannels_ {nullptr};
		ChannelPtr notifier_ {nullptr};
		FunctorList pendingFunctors_ {};
		MutexType mutex_ {};
	};

	template <typename Fn, typename... Args>
	void EventLoop::execute(Fn&& func, Args&&... args)
	{
		submit(::std::forward<Fn>(func), ::std::forward<Args>(args)...);
	}

	template <typename Fn, typename... Args, typename HasRet>
	::std::future<typename ::std::invoke_result<Fn&&, Args&&...>::type> EventLoop::submit(Fn&& func, Args&&... args)
	{
		using RetType = typename ::std::invoke_result<Fn&&, Args&&...>::type;
		auto promise = ::std::make_shared<::std::promise<RetType>>();
		auto future = promise->get_future();
		::std::function<RetType()> promiseTask = ::std::bind(::std::forward<Fn>(func), ::std::forward<Args>(args)...);
		FunctorType task = makeTask<RetType>(promise, promiseTask);
		assert(2 == promise.use_count());
		if (isInCurrentEventLoop())
		{
			task();
		}
		else
		{
			{
				LockGuardType lock(mutex_);
				pendingFunctors_.push_back(::std::move(task));
			}

		}
		return future;
	}

	template <typename Fn, typename... Args, typename HasRet>
	::std::future<void> EventLoop::submit(Fn&& func, Args&&... args)
	{
		auto promise = ::std::make_shared<::std::promise<void>>();
		auto future = promise->get_future();
		::std::function<void()> promiseTask = ::std::bind(::std::forward<Fn>(func), ::std::forward<Args>(args)...);
		FunctorType task = makeTask(promise, promiseTask);
		assert(2 == promise.use_count());
		if (isInCurrentEventLoop())
		{
			task();
		}
		else
		{
			{
				LockGuardType lock(mutex_);
				pendingFunctors_.push_back(::std::move(task));
			}

		}
		return future;
	}

	template <typename RetType>
	EventLoop::FunctorType EventLoop::makeTask(const ::std::shared_ptr<::std::promise<RetType>>& promise,
											   ::std::function<RetType()> promiseTask)
	{
		FunctorType task = [this, promise, f = ::std::move(promiseTask)]() mutable
		{
			assert(promise.use_count() > 0);
			try
			{
				RetType r = f();
				promise->set_value(r);
			}
			catch (const ::std::exception& exception)
			{
				promise->set_exception(::std::make_exception_ptr(exception));
				LOGS_ERROR << "exception caught during thread [" << nets::base::currentTid()
						   << "] execution in event loop thread [" << threadId_ << "], reason " << exception.what();
			}
			catch (...)
			{
				promise->set_exception(::std::current_exception());
				LOGS_ERROR << "exception caught during thread [" << nets::base::currentTid()
						   << "] execution in event loop thread [" << threadId_ << "]";
			}
		};
		return task;
	}

	EventLoop::FunctorType EventLoop::makeTask(const ::std::shared_ptr<::std::promise<void>>& promise,
											   ::std::function<void()> promiseTask)
	{
		FunctorType task = [this, promise, f = ::std::move(promiseTask)]() mutable
		{
			try
			{
				f();
				promise->set_value();
			}
			catch (const ::std::exception& exception)
			{
				promise->set_exception(::std::make_exception_ptr(exception));
				LOGS_ERROR << "exception caught during thread [" << nets::base::currentTid()
						   << "] execution in event loop thread [" << threadId_ << "], reason " << exception.what();
			}
			catch (...)
			{
				promise->set_exception(::std::current_exception());
				LOGS_ERROR << "exception caught during thread [" << nets::base::currentTid()
						   << "] execution in event loop thread [" << threadId_ << "]";
			}
		};
		return task;
	}
} // namespace nets::net

#endif // NETS_NET_EVENT_LOOP_H
