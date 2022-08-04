//
// Created by guang19 on 2022/4/28.
//

#ifndef NETS_BASE_COMMONMACRO_H
#define NETS_BASE_COMMONMACRO_H

#include <cstdarg>
#include <cstdio>
#include <cstring>

#include "nets/base/exception/FileCreateException.h"
#include "nets/base/exception/FileOpenException.h"

#define UNUSED(X) ((void) (X))

#define MEMZERO(P, LEN) (::memset((P), 0, (LEN)))

#define CHECK_CLASS_COMPLETE_TYPE(CLASS)                                                                                    \
    do                                                                                                                      \
    {                                                                                                                       \
        typedef char CLASS_MUST_BE_COMPLETE_TYPE[sizeof(CLASS) == 0 ? -1 : 1];                                              \
        CLASS_MUST_BE_COMPLETE_TYPE jugg;                                                                                   \
        UNUSED(jugg);                                                                                                       \
    } while (0)

template <class E>
void throwFmt(const char* fmt, ...)
{
    char msgBuf[255] = {0};
    va_list vl;
    va_start(vl, fmt);
    ::vsnprintf(msgBuf, sizeof(msgBuf), fmt, vl);
    va_end(vl);
    throw E(msgBuf);
}

#define THROW_FMT(EXCEPTION, FMT, ...) (throwFmt<EXCEPTION>(FMT "\n", ##__VA_ARGS__))

#define THROW_FILE_OPEN_EXCEPTION(ERRNUM)                                                                                   \
    switch (ERRNUM)                                                                                                         \
    {                                                                                                                       \
        case ENOENT:                                                                                                        \
            THROW_FMT(nets::base::FileOpenException, "no such file");                                                       \
            break;                                                                                                          \
        case EACCES:                                                                                                        \
        case EPERM:                                                                                                         \
            THROW_FMT(nets::base::FileOpenException, "no permission to access file");                                       \
            break;                                                                                                          \
        case EINVAL:                                                                                                        \
            THROW_FMT(nets::base::FileOpenException, "invalid value in flags");                                             \
            break;                                                                                                          \
        case EMFILE:                                                                                                        \
            THROW_FMT(nets::base::FileOpenException,                                                                        \
                      "the per-process limit on the number of open file descriptors has been reached");                     \
            break;                                                                                                          \
        case ENAMETOOLONG:                                                                                                  \
            THROW_FMT(nets::base::FileOpenException, "pathname was too long");                                              \
            break;                                                                                                          \
        default:                                                                                                            \
            THROW_FMT(nets::base::FileOpenException, "failed to open file for unknown reason,errno=%d", errNum);           \
            break;                                                                                                          \
    }

#define THROW_FILE_CREATE_EXCEPTION(ERRNUM)                                                                                 \
    switch (ERRNUM)                                                                                                         \
    {                                                                                                                       \
        case ENOENT:                                                                                                        \
            THROW_FMT(nets::base::FileCreateException, "no such file");                                                     \
            break;                                                                                                          \
        case EACCES:                                                                                                        \
        case EPERM:                                                                                                         \
            THROW_FMT(nets::base::FileCreateException, "no permission to create file");                                     \
            break;                                                                                                          \
        case EINVAL:                                                                                                        \
            THROW_FMT(nets::base::FileCreateException, "invalid value in flags");                                           \
            break;                                                                                                          \
        case EMFILE:                                                                                                        \
            THROW_FMT(nets::base::FileCreateException,                                                                      \
                      "the per-process limit on the number of open file descriptors has been reached");                     \
            break;                                                                                                          \
        case ENAMETOOLONG:                                                                                                  \
            THROW_FMT(nets::base::FileCreateException, "pathname was too long");                                            \
            break;                                                                                                          \
        default:                                                                                                            \
            THROW_FMT(nets::base::FileCreateException, "failed to create file for unknown reason,errno=%d", errNum);        \
            break;                                                                                                          \
    }

#endif // NETS_BASE_COMMONMACRO_H
