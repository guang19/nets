//
// Created by guang19 on 2021/12/30.
//

#ifndef NETS_BASE_SINGLETON_H
#define NETS_BASE_SINGLETON_H

#include <mutex>
#include "base/CommonMacro.h"
#include "base/Noncopyable.h"

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

#define DECLARE_SINGLETON_CLASS(CLASS_NAME)	\
	class CLASS_NAME : nets::base::Noncopyable


#define DEFINE_SINGLETON(CLASS_NAME) \
		\
        template<typename C>	\
		static void callAfterInit(...)	\
		{	\
		}	\
		\
		template <typename C, void (C::*)(void) = &C::afterInit>	\
		static void callAfterInit(C* c)	\
		{	\
			c->afterInit();	\
		}	\
		\
	public:	\
        template <typename ...Args>	\
		static inline CLASS_NAME*  getInstance(Args&& ...args)	\
		{	\
			::std::call_once(OnceFlag, [](Args&& ...args)	\
            {	\
                Instance = new CLASS_NAME(::std::forward<Args>(args)...);	\
				callAfterInit<CLASS_NAME>(Instance);	\
                atexit(destroy);	\
            }, ::std::forward<Args>(args)...);	\
            return Instance;	\
		}	\
		\
	private:	\
		static void destroy()	\
		{	\
			typedef char T_must_be_complete_type[sizeof(CLASS_NAME) == 0 ? -1 : 1];	\
			T_must_be_complete_type jugg;	\
			UNUSED(jugg);	\
			if (Instance != nullptr)	\
			{	\
				delete Instance;	\
				Instance = nullptr;	\
			}	\
		}	\
		\
	private:	\
		static CLASS_NAME* Instance;	\
		static ::std::once_flag OnceFlag

#define INIT_SINGLETON(CLASS_NAME)	\
	CLASS_NAME* CLASS_NAME::Instance { nullptr };	\
	::std::once_flag CLASS_NAME::OnceFlag {}

//namespace nets
//{
//    namespace base
//    {
//        template<class T>
//        class Singleton : Noncopyable
//        {
//            public:
//                template<typename ...Args>
//                static T* getInstance(Args&& ...args);
//
//            private:
//                static void destroy()
//				{
//					// singleton class must be complete type
//					typedef char T_must_be_complete_type[sizeof(T) == 0 ? -1 : 1];
//					T_must_be_complete_type jugg;
//					UNUSED(jugg);
//					if (value_ != nullptr)
//					{
//						delete value_;
//						value_ = nullptr;
//					}
//				}
//
//            private:
//                static T* value_;
//                static ::std::once_flag onceFlag_;
//        };
//
//        template <class T>
//        T* Singleton<T>::value_ { nullptr };
//
//        template <class T>
//        ::std::once_flag Singleton<T>::onceFlag_ {};
//
//        template<class T>
//        template<typename ...Args>
//        T* Singleton<T>::getInstance(Args&& ...args)
//        {
//            ::std::call_once(onceFlag_, [](Args&& ...args)
//            {
//                value_ = new T(::std::forward<Args>(args)...);
//				// auto free
//                atexit(destroy);
//            }, ::std::forward<Args>(args)...);
//            return value_;
//        }
//    } // namespace base
//} // namespace nets

#endif // NETS_BASE_SINGLETON_H