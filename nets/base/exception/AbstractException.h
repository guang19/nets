//
// Created by guang19
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

        inline const StringType& stackTrace() const noexcept
        {
            return stackTrace_;
        }

    private:
        void backTrace() noexcept;

    protected:
        StringType cause_ {};
        StringType stackTrace_ {};
    };
} // namespace nets::base

#endif // NETS_BASE_ABSTRACT_EXCEPTION_H