//
// Created by guang19
//

#ifndef NETS_FILE_CREATE_EXCEPTION_H
#define NETS_FILE_CREATE_EXCEPTION_H

#include "nets/base/exception/AbstractException.h"

namespace nets
{
    class FileCreateException : public AbstractException
    {
    public:
        FileCreateException() : AbstractException() {}
        explicit FileCreateException(const StringType& cause) : AbstractException(cause) {}
        ~FileCreateException() override = default;
    };
} // namespace nets

#endif // NETS_FILE_CREATE_EXCEPTION_H