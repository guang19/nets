//
// Created by guang19 on 2022/8/4.
//

#ifndef NETS_BASE_ABSTRACT_EXCEPTION_H
#define NETS_BASE_ABSTRACT_EXCEPTION_H

#include <exception>
#include <string>

#include "nets/base/Noncopyable.h"

namespace nets::base
{
    class AbstractException : public ::std::exception, Noncopyable
    {
    public:
        AbstractException();
        ~AbstractException() noexcept override = default;

    public:
        const ::std::string& stackTrace()
        {
            return stackTrace_;
        }

    private:
        void backTrace() noexcept;

    private:
        ::std::string stackTrace_ {};
    };
} // namespace nets::base

#endif // NETS_BASE_ABSTRACT_EXCEPTION_H
