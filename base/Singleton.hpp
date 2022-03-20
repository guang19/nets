//
// Created by guang19 on 2021/12/30.
//

#ifndef NETS_BASE_SINGLETON_H
#define NETS_BASE_SINGLETON_H

#include <mutex>
#include "base/Noncopyable.h"

namespace nets
{
    namespace base
    {
        template<class T>
        class Singleton : Noncopyable
        {
            public:
                template<typename ...Args>
                static T* getInstance(Args&& ...args);

            private:
                static void destroy();

            private:
                static T* value_;
                static ::std::once_flag onceFlag_;
        };

        template <class T>
        T* Singleton<T>::value_ = nullptr;

        template <class T>
        ::std::once_flag Singleton<T>::onceFlag_;

        template<class T>
        void Singleton<T>::destroy()
        {
			// 单例类型必须是完整类型
            typedef char T_must_be_complete_type[sizeof(T) == 0 ? -1 : 1];
            T_must_be_complete_type jugg;
            (void) jugg;
            if (value_ != nullptr)
            {
                delete value_;
                value_ = nullptr;
            }
        }

        template<class T>
        template<typename ...Args>
        T* Singleton<T>::getInstance(Args&& ...args)
        {
            ::std::call_once(onceFlag_, [](Args&& ...args)
            {
                value_ = new T(::std::forward<Args>(args)...);
				// 自动释放内存
                atexit(destroy);
            }, ::std::forward<Args>(args)...);
            return value_;
        }
    } // namespace base
} // namespace nets

#endif // NETS_BASE_SINGLETON_H