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
 *	If you need to do some work before the singleton is initialized, you need to define a function
 *	named <beforeInit>. function is automatically called when the singleton is initialized.
 *  eg: RetType beforeInit(void);
 *
 *	If you need to do some work before the singleton is initialized, you need to define a function
 *	named <afterInit>, function is automatically called after the singleton is initialized.
 *	eg: RetType afterInit(void);
 */

#define DECLARE_SINGLETON_CLASS(CLASS_NAME)	\
	class CLASS_NAME : nets::base::Noncopyable


#define DEFINE_SINGLETON(CLASS_NAME) \
		DECLARE_HAS_MEMBER_FUNCTION(beforeInit);	\
		DECLARE_HAS_MEMBER_FUNCTION(afterInit);	\
		\
	protected:	\
		CLASS_NAME() = default;	\
		~CLASS_NAME() = default;	\
		\
		template<typename C = CLASS_NAME>	\
		void callBeforeInit(...)	\
		{	\
		}	\
			\
		template <typename C = CLASS_NAME, void (C::*)() = &C::beforeInit>	\
		void callBeforeInit(const void*)	\
		{	\
			beforeInit();	\
		}	\
		\
        template<typename C = CLASS_NAME>	\
		void callAfterInit(...)	\
		{	\
		}	\
			\
		template <typename C = CLASS_NAME, void (C::*)() = &C::afterInit>	\
		void callAfterInit(const void*)	\
		{	\
			afterInit();	\
		}	\
		\
	public:	\
        template <typename ...Args>	\
		static inline CLASS_NAME*  getInstance(Args&& ...args)	\
		{	\
			::std::call_once(OnceFlag, [](Args&& ...args)	\
            {	\
				if (HAS_MEMBER_FUNCTION(CLASS_NAME, beforeInit))	\
				{	\
            		callBeforeInit(nullptr);	\
				}	\
                Instance = new CLASS_NAME(::std::forward<Args>(args)...);	\
                if (HAS_MEMBER_FUNCTION(CLASS_NAME, afterInit))	\
                {	\
					callAfterInit(nullptr);	\
                }	\
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
//					// 单例类型必须是完整类型
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