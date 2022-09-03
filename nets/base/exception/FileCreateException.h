//
// Created by guang19 on 2022/9/3.
//

#ifndef NETS_BASE_FILE_CREATE_EXCEPTION_H
#define NETS_BASE_FILE_CREATE_EXCEPTION_H

#include "nets/base/exception/AbstractException.h"

namespace nets::base
{
    class FileCreateException : public AbstractException
    {
    public:
        FileCreateException() : AbstractException() {}
        explicit FileCreateException(const StringType& cause) : AbstractException(cause) {}
        ~FileCreateException() override = default;
    };
} // namespace nets::base

#endif // NETS_BASE_FILE_CREATE_EXCEPTION_H