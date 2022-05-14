//
// Created by YangGuang on 2022/4/2.
//

#include <gtest/gtest.h>

#include <algorithm>
#include <cmath>

#include "nets/base/ByteStream.h"

using namespace nets::base;

TEST(ByteStreamTest, ByteBufferTest)
{
	ByteStream stream1(1024);
	stream1 << "abcdefg";
	ByteStream stream2(stream1);
	ASSERT_EQ(stream1.readerIndex(), stream2.readerIndex());
	ASSERT_EQ(stream1.writerIndex(), stream2.writerIndex());
	ASSERT_EQ(stream1.capacity(), stream2.capacity());
	ByteStream stream3(::std::move(stream2));
	ASSERT_EQ(stream1.readerIndex(), stream3.readerIndex());
	ASSERT_EQ(stream1.writerIndex(), stream3.writerIndex());
	ASSERT_EQ(stream1.capacity(), stream3.capacity());
	ASSERT_EQ(stream2.readerIndex(), 0U);
	ASSERT_EQ(stream2.writerIndex(), 0U);
	ASSERT_EQ(stream2.capacity(), 0U);
	ByteStream stream4 = ::std::move(stream3);
	ASSERT_EQ(stream1.readerIndex(), stream4.readerIndex());
	ASSERT_EQ(stream1.writerIndex(), stream4.writerIndex());
	ASSERT_EQ(stream1.capacity(), stream4.capacity());
	ASSERT_EQ(stream3.readerIndex(), 0U);
	ASSERT_EQ(stream3.writerIndex(), 0U);
	ASSERT_EQ(stream3.capacity(), 0U);
}

TEST(ByteStreamTest, Append)
{
	ByteStream stream {1024};
	ASSERT_EQ(stream.writerIndex(), 0U);
	stream << "abc";
	ASSERT_EQ(stream.writerIndex(), 3U);
	int8_t i8 = 8;
	stream << i8 << &i8 << 12345678U << NAN << 123123.5123123F << 123123123.12312312 << "我爱你 I love you";
	::printf("%u\n", stream.writerIndex());
	::printf("%u\n", stream.writeableBytes());
}

TEST(ByteStreamTest, AppendBuffer)
{
	ByteStream stream {1024};
	ASSERT_EQ(stream.writerIndex(), 0U);
	stream << "abc";
	ASSERT_EQ(stream.writerIndex(), 3U);
	ByteStream stream2 {1024};
	stream2 << "abc";
	stream << stream2;
	ASSERT_EQ(stream.writerIndex(), 6U);
	ASSERT_EQ(stream.writeableBytes(), 1024U - 6U);
}

int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}