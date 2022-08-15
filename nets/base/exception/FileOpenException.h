//
// Created by guang19
//

#ifndef NETS_BASE_FILE_OPEN_EXCEPTION_H
#define NETS_BASE_FILE_OPEN_EXCEPTION_H

#include "nets/base/exception/AbstractException.h"

namespace nets::base
{
    class FileOpenException : public AbstractException
    {
    public:
        FileOpenException() : AbstractException() {}
        explicit FileOpenException(const StringType& cause) : AbstractException(cause) {}
        ~FileOpenException() override = default;
    };
} // namespace nets::base

#endif // NETS_BASE_FILE_OPEN_EXCEPTION_H