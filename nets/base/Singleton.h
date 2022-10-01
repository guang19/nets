//
// Created by guang19
//

#ifndef NETS_SINGLETON_H
#define NETS_SINGLETON_H

#include <memory>
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
 *	named <afterInit>, the function named afterInit is automatically called after the singleton is initialized.
 *
 * return type and parameter are void
 * /////////////////////////
 *	void afterInit(void);
 * /////////////////////////
 * eg:
 * DECLARE_SINGLETON_CLASS(MyClass)
 * {
 *     DEFINE_SINGLETON(MyClass)
 *
 *     ....
 *
 * 	   private:
 * 	   	   void start()
 * 	   	   {
 * 	   	       ::printf("Hello, start after you getInstance(...)");
 * 	   	   }
 *
 * 	       // will be called after the singleton is initialized
 * 	       void afterInit()
 * 	       {
 * 	       	   // doSomething
 * 	           this->start();
 * 	       }
 * };
 *
 * INIT_SINGLETON(MyClass);
 *
 * auto myClass = MyClass::getInstance(...);
 *
 */

#define DECLARE_SINGLETON_CLASS(CLASS_NAME) class CLASS_NAME : nets::Noncopyable

#define DEFINE_SINGLETON(CLASS_NAME)                                                                                        \
                                                                                                                            \
private:                                                                                                                    \
    template <typename C>                                                                                                   \
    static void _CALL_AFTER_INIT(...)                                                                                       \
    {                                                                                                                       \
    }                                                                                                                       \
                                                                                                                            \
    template <typename C, void (C::*)(void) = &C::afterInit>                                                                \
    static void _CALL_AFTER_INIT(C* c)                                                                                      \
    {                                                                                                                       \
        c->afterInit();                                                                                                     \
    }                                                                                                                       \
                                                                                                                            \
    static void _DESTROY(CLASS_NAME* instance)                                                                              \
    {                                                                                                                       \
        delete instance;                                                                                                    \
    }                                                                                                                       \
                                                                                                                            \
    template <typename... Args>                                                                                             \
    static void _INIT(Args&&... args)                                                                                       \
    {                                                                                                                       \
        CHECK_CLASS_COMPLETE_TYPE(CLASS_NAME);                                                                              \
        gINSTANCE = ::std::shared_ptr<CLASS_NAME>(new CLASS_NAME(::std::forward<Args>(args)...), &CLASS_NAME::_DESTROY);    \
        _CALL_AFTER_INIT<CLASS_NAME>(gINSTANCE.get());                                                                      \
    }                                                                                                                       \
                                                                                                                            \
public:                                                                                                                     \
    template <typename... Args>                                                                                             \
    static inline ::std::shared_ptr<CLASS_NAME> getInstance(Args&&... args)                                                 \
    {                                                                                                                       \
        ::std::call_once(                                                                                                   \
            gONCE_FLAG,                                                                                                     \
            [](Args&&... args0)                                                                                             \
            {                                                                                                               \
                if (nullptr == gINSTANCE)                                                                                   \
                {                                                                                                           \
                    CLASS_NAME::_INIT(::std::forward<Args>(args0)...);                                                      \
                }                                                                                                           \
            },                                                                                                              \
            ::std::forward<Args>(args)...);                                                                                 \
        return gINSTANCE;                                                                                                   \
    }                                                                                                                       \
                                                                                                                            \
private:                                                                                                                    \
    static ::std::shared_ptr<CLASS_NAME> gINSTANCE;                                                                         \
    static ::std::once_flag gONCE_FLAG

#define INIT_SINGLETON(CLASS_NAME)                                                                                          \
    ::std::shared_ptr<CLASS_NAME> CLASS_NAME::gINSTANCE {nullptr};                                                          \
    ::std::once_flag CLASS_NAME::gONCE_FLAG                                                                                 \
    {                                                                                                                       \
    }

#endif // NETS_SINGLETON_H