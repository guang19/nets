// MIT License
//
// Copyright (c) 2022 guang19
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

// @brief Once flag based singleton

#ifndef NETS_SINGLETON_H
#define NETS_SINGLETON_H

#include <memory>
#include <mutex>

#include "nets/base/Noncopyable.h"

/***
 *
 * Considering the problem of cross-DLL calls in templates,
 * i did not use templates to write singletons.
 * If a class needs to use a singleton, use the following three macros directly.
 *
 * If you need to do some work after the singleton is initialized, you need to define a function
 * named <afterInit>, the function named afterInit is automatically called after the singleton is initialized.
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
 * INIT_SINGLETON(MyClass); // INIT_SINGLETON in .cpp file
 *
 * auto myClass = MyClass::getInstance(...);
 *
 */

#define DECLARE_SINGLETON_CLASS(CLASS_NAME) class CLASS_NAME : nets::Noncopyable

#define DEFINE_SINGLETON(CLASS_NAME)                                                                                        \
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
        _INSTANCE = ::std::shared_ptr<CLASS_NAME>(new CLASS_NAME(::std::forward<Args>(args)...), &CLASS_NAME::_DESTROY);    \
        _CALL_AFTER_INIT<CLASS_NAME>(_INSTANCE.get());                                                                      \
    }                                                                                                                       \
                                                                                                                            \
public:                                                                                                                     \
    template <typename... Args>                                                                                             \
    static inline ::std::shared_ptr<CLASS_NAME> getInstance(Args&&... args)                                                 \
    {                                                                                                                       \
        ::std::call_once(                                                                                                   \
            _ONCE_FLAG,                                                                                                     \
            [](Args&&... args0)                                                                                             \
            {                                                                                                               \
                if (nullptr == _INSTANCE)                                                                                   \
                {                                                                                                           \
                    CLASS_NAME::_INIT(::std::forward<Args>(args0)...);                                                      \
                }                                                                                                           \
            },                                                                                                              \
            ::std::forward<Args>(args)...);                                                                                 \
        return _INSTANCE;                                                                                                   \
    }                                                                                                                       \
                                                                                                                            \
private:                                                                                                                    \
    static ::std::shared_ptr<CLASS_NAME> _INSTANCE;                                                                         \
    static ::std::once_flag _ONCE_FLAG

#define INIT_SINGLETON(CLASS_NAME)                                                                                          \
    ::std::shared_ptr<CLASS_NAME> CLASS_NAME::_INSTANCE {nullptr};                                                          \
    ::std::once_flag CLASS_NAME::_ONCE_FLAG {}

#endif // NETS_SINGLETON_H