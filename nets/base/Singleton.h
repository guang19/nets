//
// Created by guang19 on 2021/12/30.
//

#ifndef NETS_BASE_SINGLETON_H
#define NETS_BASE_SINGLETON_H

#include <functional>
#include <pthread.h>

#include "nets/base/CommonMacro.h"
#include "nets/base/Noncopyable.h"

/***
 *
 *	Considering the problem of cross-DLL calls in templates,
 *	i did not use templates to write singletons.
 *  If a class needs to use a singleton, use the following three macros directly.
 *
 *	If you need to do some work after the singleton is initialized, you need to define a function
 *	named <afterInit>, function is automatically called after the singleton is initialized.
 *
 * return type and parameter are void
 * /////////////////////////
 *	void afterInit(void);
 * /////////////////////////
 *
 */

#define DECLARE_SINGLETON_CLASS(CLASS_NAME) class CLASS_NAME : nets::base::Noncopyable

#define DEFINE_SINGLETON(CLASS_NAME)                                                                                        \
                                                                                                                            \
protected:                                                                                                                  \
	CLASS_NAME() = default;                                                                                                 \
	~CLASS_NAME() = default;                                                                                                \
                                                                                                                            \
	template <typename C>                                                                                                   \
	static void callAfterInit(...)                                                                                          \
	{                                                                                                                       \
	}                                                                                                                       \
                                                                                                                            \
	template <typename C, void (C::*)(void) = &C::afterInit>                                                                \
	static void callAfterInit(C* c)                                                                                         \
	{                                                                                                                       \
		c->afterInit();                                                                                                     \
	}                                                                                                                       \
                                                                                                                            \
	static void init()                                                                                                      \
	{                                                                                                                       \
		Init##CLASS_NAME##InstanceFunc();                                                                                   \
	}                                                                                                                       \
                                                                                                                            \
	static void destroy()                                                                                                   \
	{                                                                                                                       \
		typedef char T_must_be_complete_type[sizeof(CLASS_NAME) == 0 ? -1 : 1];                                             \
		T_must_be_complete_type jugg;                                                                                       \
		UNUSED(jugg);                                                                                                       \
		if (Instance != nullptr)                                                                                            \
		{                                                                                                                   \
			delete Instance;                                                                                                \
			Instance = nullptr;                                                                                             \
		}                                                                                                                   \
	}                                                                                                                       \
                                                                                                                            \
public:                                                                                                                     \
	template <typename... Args>                                                                                             \
	static inline CLASS_NAME* getInstance(Args&&... args)                                                                   \
	{                                                                                                                       \
		Init##CLASS_NAME##InstanceFunc = ::std::bind(                                                                       \
			[&]()                                                                                                           \
			{                                                                                                               \
				Instance = new CLASS_NAME(::std::forward<Args>(args)...);                                                   \
				callAfterInit<CLASS_NAME>(Instance);                                                                        \
				::atexit(&CLASS_NAME::destroy);                                                                             \
			});                                                                                                             \
		::pthread_once(&OnceFlag, &CLASS_NAME::init);                                                                       \
		return Instance;                                                                                                    \
	}                                                                                                                       \
                                                                                                                            \
private:                                                                                                                    \
	static CLASS_NAME* Instance;                                                                                            \
	static ::pthread_once_t OnceFlag;                                                                                       \
	static ::std::function<void()> Init##CLASS_NAME##InstanceFunc

#define INIT_SINGLETON(CLASS_NAME)                                                                                          \
	CLASS_NAME* CLASS_NAME::Instance {nullptr};                                                                             \
	::pthread_once_t CLASS_NAME::OnceFlag {PTHREAD_ONCE_INIT};                                                              \
	::std::function<void()> CLASS_NAME::Init##CLASS_NAME##InstanceFunc {}

#endif // NETS_BASE_SINGLETON_H