//
// Created by n021949 on 2022/4/28.
//

#ifndef NETS_COMMONMACRO_H
#define NETS_COMMONMACRO_H

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

#endif // NETS_COMMONMACRO_H
