//
// Created by guang19 on 2022/8/4.
//

#ifndef NETS_BASE_FILE_CREATION_EXCEPTION_H
#define NETS_BASE_FILE_CREATION_EXCEPTION_H

#include "nets/base/exception/AbstractException.h"

namespace nets::base
{
    class FileCreationException : public AbstractException
    {
    public:
        FileCreationException() : AbstractException() {}
        explicit FileCreationException(const StringType& cause) : AbstractException(cause) {}
        ~FileCreationException() override = default;
    };
} // namespace nets::base

#endif // NETS_BASE_FILE_CREATION_EXCEPTION_H
