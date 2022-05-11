//
// Created by n021949 on 2022/4/28.
//

#ifndef NETS_COMMONMACRO_H
#define NETS_COMMONMACRO_H

#include <cstring>
#include <utility>

#define UNUSED(x) ((void) (x))

#define MEMZERO(p, len) (::memset((p), 0, (len)))

#define CHECK_CLASS_COMPLETE_TYPE(CLASS)                                                                                    \
	typedef char CLASS_MUST_BE_COMPLETE_TYPE[sizeof(CLASS) == 0 ? -1 : 1];                                                  \
	CLASS_MUST_BE_COMPLETE_TYPE jugg;                                                                                       \
	UNUSED(jugg)

#define AVAILABLE_PROCESSOR (::sysconf(_SC_NPROCESSORS_ONLN))

#define DECLARE_HAS_MEMBER_FUNCTION(FUNC)                                                                                   \
	template <class C, typename... Args>                                                                                    \
	static bool HasMemberFunc_##FUNC(decltype(::std::declval<C>().FUNC(::std::declval<Args>()...))* = nullptr)              \
	{                                                                                                                       \
		return true;                                                                                                        \
	}                                                                                                                       \
                                                                                                                            \
	template <class C, typename... Args>                                                                                    \
	static bool HasMemberFunc_##FUNC(...)                                                                                   \
	{                                                                                                                       \
		return false;                                                                                                       \
	}

// check if the class has the function
#define HAS_MEMBER_FUNCTION(CLASS, FUNC, ...) HasMemberFunc_##FUNC<CLASS, ##__VA_ARGS__>(nullptr)

#endif // NETS_COMMONMACRO_H
