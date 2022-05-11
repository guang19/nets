//
// Created by guang19 on 2021/12/30.
//

#ifndef NETS_BASE_SINGLETON_H
#define NETS_BASE_SINGLETON_H

#include <mutex>

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
	template <typename... Args>                                                                                             \
	static void init(Args&&... args)                                                                                        \
	{                                                                                                                       \
		Instance = new CLASS_NAME(::std::forward<Args>(args)...);                                                           \
		callAfterInit<CLASS_NAME>(Instance);                                                                                \
	}                                                                                                                       \
                                                                                                                            \
	static void destroy()                                                                                                   \
	{                                                                                                                       \
		CHECK_CLASS_COMPLETE_TYPE(CLASS_NAME);                                                                              \
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
		::std::call_once(                                                                                                   \
			OnceFlag,                                                                                                       \
			[](Args&&... args0)                                                                                             \
			{                                                                                                               \
				if (Instance == nullptr)                                                                                    \
				{                                                                                                           \
					CLASS_NAME::init(::std::forward<Args>(args0)...);                                                       \
				}                                                                                                           \
				::atexit(&CLASS_NAME::destroy);                                                                             \
			},                                                                                                              \
			::std::forward<Args>(args)...);                                                                                 \
		return Instance;                                                                                                    \
	}                                                                                                                       \
                                                                                                                            \
private:                                                                                                                    \
	static CLASS_NAME* Instance;                                                                                            \
	static ::std::once_flag OnceFlag

#define INIT_SINGLETON(CLASS_NAME)                                                                                          \
	CLASS_NAME* CLASS_NAME::Instance {nullptr};                                                                             \
	::std::once_flag CLASS_NAME::OnceFlag {}

#endif // NETS_BASE_SINGLETON_H