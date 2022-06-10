//
// Created by guang19 on 2022/6/9.
//

#ifndef NETS_STRINGUTILS_H
#define NETS_STRINGUTILS_H

#include <algorithm>
#include <cmath>
#include <cstdint>

namespace nets::base
{
	namespace
	{
		constexpr char Digits[] = {"0123456789"};
		constexpr char HexDigits[] = {"0123456789abcdef"};
		constexpr uint32_t MaxNumLen = ::std::numeric_limits<uint64_t>::digits10 + 2;
		constexpr uint32_t MaxFloatLen = ::std::numeric_limits<double>::digits10 + 2;
	} // namespace

	namespace utils
	{
		// Convert the integer to a string and add it to the buffer, and return the length of the converted string
		template <typename Int>
		uint32_t fromInt(char* buffer, Int n)
		{
			char* tmp = buffer;
			do
			{
				uint16_t lstIdx = static_cast<uint16_t>(n % 10);
				*tmp = Digits[lstIdx];
				++tmp;
				n /= 10;
			} while (n > 0);
			if (n < 0)
			{
				*tmp = '-';
				++tmp;
			}
			::std::reverse(buffer, tmp);
			return tmp - buffer;
		}

		// Convert the float to a string and add it to the buffer, and return the length of the converted string
		template <typename Float>
		uint32_t fromFloat(char* buffer, Float f)
		{
			if (::std::isnan(f))
			{
				::memcpy(buffer, "nan", 3);
				return 3;
			}
			else if (::std::isinf(f))
			{
				::memcpy(buffer, "inf", 3);
				return 3;
			}
			else
			{
				return ::snprintf(buffer, MaxFloatLen, "%.17g", f);
			}
		}

		// Convert the hex to a string and add it to the buffer, and return the length of the converted string
		template <typename Hex>
		uint32_t fromHex(char* buffer, Hex h)
		{
			char* tmp = buffer;
			tmp[0] = '0';
			tmp[1] = 'x';
			tmp += 2;
			do
			{
				uint16_t lstIdx = static_cast<uint16_t>(h % 16);
				*tmp = HexDigits[lstIdx];
				++tmp;
				h /= 16;
			} while (h > 0);
			::std::reverse(buffer + 2, tmp);
			return tmp - buffer;
		}
	} // namespace utils
} // namespace nets::base

#endif // NETS_STRINGUTILS_H
