//
// Created by YangGuang on 2022/5/8.
//

#ifndef NETS_BYTESTREAM_H
#define NETS_BYTESTREAM_H

#include <cstdint>
#include <cstring>
#include <string>
#include "nets/base/ByteBuffer.h"
#include "nets/base/Noncopyable.h"


namespace nets
{
	namespace base
	{
		class ByteStream : public ByteBuffer, Noncopyable
		{
			public:
				explicit ByteStream(SizeTp capacity) : ByteBuffer(capacity)
				{
				}

				~ByteStream() override = default;

			public:
				template <typename Number>
				void appendInteger(Number n);

				template<typename Float>
				void appendFloat(Float f);

				void appendStr(const char* data, uint32_t len);
				void appendPointer(uintptr_t ptr);

			public:
				ByteStream& operator<<(char c);
				ByteStream& operator<<(int8_t n);
				ByteStream& operator<<(uint8_t n);
				ByteStream& operator<<(int16_t n);
				ByteStream& operator<<(uint16_t n);
				ByteStream& operator<<(int32_t n);
				ByteStream& operator<<(uint32_t n);
				ByteStream& operator<<(int64_t n);
				ByteStream& operator<<(uint64_t n);
				ByteStream& operator<<(const void* ptr);
				ByteStream& operator<<(float f);
				ByteStream& operator<<(double f);
				ByteStream& operator<<(const char* str);
				ByteStream& operator<<(const ::std::string& str);
				ByteStream& operator<<(const ByteStream& stream);
		};
	} // namespace base
} // namespace nets

#endif //NETS_BYTESTREAM_H
