//
// Created by n021949 on 2022/4/28.
//

#ifndef NETS_BASE_COMMONMACRO_H
#define NETS_BASE_COMMONMACRO_H

#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <unistd.h>

#define UNUSED(x) ((void) (x))

#define MEMZERO(p, len) (::memset((p), 0, (len)))

#define AVAILABLE_PROCESSOR (::sysconf(_SC_NPROCESSORS_ONLN))

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

#define THROW_FMT(EXCEPTION, fmt, ...) (throwFmt<EXCEPTION>(fmt"\n", ##__VA_ARGS__))

#endif // NETS_BASE_COMMONMACRO_H
