//
// Created by n021949 on 2022/4/28.
//

#ifndef NETS_COMMONMACRO_H
#define NETS_COMMONMACRO_H

#include <utility>

#define UNUSED(x) ((void) (x))

/***********************************
  *
#define DECLARE_HAS_MEMBER_FUNCTION(FUNC) \
    template<class T, typename ...Args>	\
	struct HasMemberFunc_##FUNC	\
	{	\
		private:	\
			template<class C>	\
			constexpr static auto check(const void*) ->	\
				decltype(::std::declval<C>().FUNC(::std::declval<Args>()...), ::std::true_type());	\
				\
			template<class C>	\
			constexpr static ::std::false_type check(...);	\
			\
		public:	\
			constexpr static bool value = decltype(check<T>(nullptr))::value;	\
	}

// check if the class has the function
#define HAS_MEMBER_FUNCTION(CLASS, FUNC, ...)	\
		HasMemberFunc_##FUNC<CLASS, ##__VA_ARGS__>::value
 *
 ***********************************/

#define DECLARE_HAS_MEMBER_FUNCTION(FUNC) \
    template<class C, typename ...Args>      \
    static bool HasMemberFunc_##FUNC(decltype(::std::declval<C>().FUNC(::std::declval<Args>()...))* = nullptr) \
    {    \
        return true;    \
    }    \
        \
    template<class C, typename ...Args>    \
    static bool HasMemberFunc_##FUNC(...)    \
    {    \
        return false;    \
    }

// check if the class has the function
#define HAS_MEMBER_FUNCTION(CLASS, FUNC, ...) \
    HasMemberFunc_##FUNC<CLASS, ##__VA_ARGS__>(nullptr)


#endif //NETS_COMMONMACRO_H
