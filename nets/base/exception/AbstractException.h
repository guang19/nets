//
// Created by guang19
//

#ifndef NETS_ABSTRACT_EXCEPTION_H
#define NETS_ABSTRACT_EXCEPTION_H

#include <exception>
#include <string>

#include "nets/base/Noncopyable.h"

namespace nets
{
    class AbstractException : public ::std::exception, Noncopyable
    {
    public:
        using StringType = ::std::string;

    protected:
        AbstractException();
        explicit AbstractException(const StringType& cause);
        ~AbstractException() override = default;

    public:
        inline const char* what() const noexcept override
        {
            return cause_.c_str();
        }

        inline const StringType& backtrace() const noexcept
        {
            return backtraceInfo_;
        }

    protected:
        StringType cause_;
        StringType backtraceInfo_;
    };
} // namespace nets

#endif // NETS_ABSTRACT_EXCEPTION_H