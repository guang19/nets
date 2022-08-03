//
// Created by guang19 on 2022/8/4.
//

#include <gtest/gtest.h>

#include "nets/net/core/ByteBuffer.h"

using namespace nets::net;

TEST(ByteBufferTest, Constructor)
{
    ByteBuffer buffer {};
    ASSERT_EQ(buffer.capacity(), 1024U);
    ByteBuffer buffer2 {512};
    ByteBuffer buffer3(512);
    ASSERT_EQ(buffer.capacity(), 1024U);
    ASSERT_EQ(buffer2.capacity(), 512U);
    ASSERT_EQ(buffer3.capacity(), 512U);
    ASSERT_FALSE(buffer.isReadable());
    ASSERT_TRUE(buffer.isWritable());
}


TEST(ByteBufferTest, WriteBytes)
{
    ByteBuffer buffer {};
    const char* s1 = "123";
    buffer.writeBytes(s1, ::strlen(s1));
    ASSERT_TRUE(buffer.isReadable());
    ASSERT_EQ(buffer.readableBytes(), ::strlen(s1));
    ::std::string s = buffer.readBytes(::strlen(s1));
    ASSERT_STREQ(s.c_str(), s1);
    ASSERT_FALSE(buffer.isReadable());
    ASSERT_EQ(buffer.readerIndex(), 0U);
    ASSERT_EQ(buffer.writerIndex(), 0U);
}

TEST(ByteBufferTest, WriteInt16)
{
    ByteBuffer buffer {};
    int16_t n = 1024;
    buffer.writeInt16(n);
    ASSERT_TRUE(buffer.isReadable());
    ASSERT_EQ(buffer.readableBytes(), sizeof(int16_t));
    ASSERT_EQ(buffer.readInt16(), n);
    ASSERT_FALSE(buffer.isReadable());
    ASSERT_EQ(buffer.readerIndex(), 0U);
    ASSERT_EQ(buffer.writerIndex(), 0U);
}

TEST(ByteBufferTest, WriteInt32)
{
    ByteBuffer buffer {};
    int32_t n = 2048;
    buffer.writeInt32(n);
    ASSERT_TRUE(buffer.isReadable());
    ASSERT_EQ(buffer.readableBytes(), sizeof(int32_t));
    ASSERT_EQ(buffer.readInt32(), n);
    ASSERT_FALSE(buffer.isReadable());
    ASSERT_EQ(buffer.readerIndex(), 0U);
    ASSERT_EQ(buffer.writerIndex(), 0U);
}

TEST(ByteBufferTest, WriteInt64)
{
    ByteBuffer buffer {};
    int64_t n = 65535;
    buffer.writeInt64(n);
    ASSERT_TRUE(buffer.isReadable());
    ASSERT_EQ(buffer.readableBytes(), sizeof(int64_t));
    ASSERT_EQ(buffer.readInt64(), n);
    ASSERT_FALSE(buffer.isReadable());
    ASSERT_EQ(buffer.readerIndex(), 0U);
    ASSERT_EQ(buffer.writerIndex(), 0U);
}

TEST(ByteBufferTest, WriteIntFloat)
{
    ByteBuffer buffer {};
    float n = 123.123F;
    buffer.writeFloat(n);
    ASSERT_TRUE(buffer.isReadable());
    ASSERT_EQ(buffer.readableBytes(), sizeof(float));
    ASSERT_EQ(buffer.readFloat(), n);
    ASSERT_FALSE(buffer.isReadable());
    ASSERT_EQ(buffer.readerIndex(), 0U);
    ASSERT_EQ(buffer.writerIndex(), 0U);
}

TEST(ByteBufferTest, WriteIntDouble)
{
    ByteBuffer buffer {};
    double n = 123.1234567891012345678910L;
    buffer.writeDouble(n);
    ASSERT_TRUE(buffer.isReadable());
    ASSERT_EQ(buffer.readableBytes(), sizeof(double));
    ASSERT_EQ(buffer.readDouble(), n);
    ASSERT_FALSE(buffer.isReadable());
    ASSERT_EQ(buffer.readerIndex(), 0U);
    ASSERT_EQ(buffer.writerIndex(), 0U);
}

TEST(ByteBufferTest, Comprehensive)
{
    ByteBuffer buffer {};
    const char* s = "Hello World,你好，世界！";
    buffer.writeBytes(s, ::strlen(s));
    buffer.writeByte('C');
    buffer.writeInt8(127);
    buffer.writeInt16(32767);
    buffer.writeInt32(123123123);
    buffer.writeInt64(123123123123);
    ASSERT_STREQ(buffer.readBytes(::strlen(s)).c_str(), s);
    ASSERT_EQ(buffer.readByte(), 'C');
    ASSERT_EQ(buffer.readInt8(), 127);
    ASSERT_EQ(buffer.readInt16(), 32767);
    ASSERT_EQ(buffer.readInt32(), 123123123);
    ASSERT_EQ(buffer.readInt64(), 123123123123);
    ASSERT_FALSE(buffer.isReadable());
}

TEST(ByteBufferTest, Copy)
{
    ByteBuffer buffer1 {};
    const char* s = "Hello World,你好，世界！";
    buffer1.writeBytes(s, ::strlen(s));
    buffer1.writeByte('C');
    buffer1.writeInt8(127);
    buffer1.writeInt16(32767);
    buffer1.writeInt32(123123123);
    buffer1.writeInt64(123123123123);
    ByteBuffer buffer2(buffer1);
    ASSERT_EQ(buffer1.readerIndex(), buffer2.readerIndex());
    ASSERT_EQ(buffer1.writerIndex(), buffer2.writerIndex());
    ASSERT_EQ(buffer1.capacity(), buffer2.capacity());
    ASSERT_STREQ(buffer1.readBytes(::strlen(s)).c_str(), buffer2.readBytes(::strlen(s)).c_str());
    ASSERT_EQ(buffer1.readByte(), buffer2.readByte());
    ASSERT_EQ(buffer1.readInt8(), buffer2.readInt8());
    ASSERT_EQ(buffer1.readInt16(), buffer2.readInt16());
    ASSERT_EQ(buffer1.readInt32(), buffer2.readInt32());
    ASSERT_EQ(buffer1.readInt64(), buffer2.readInt64());
    ASSERT_FALSE(buffer1.isReadable());
    ASSERT_FALSE(buffer2.isReadable());
}

TEST(ByteBufferTest, Move)
{
    ByteBuffer buffer1 {};
    const char* s = "Hello World,你好，世界！";
    buffer1.writeBytes(s, ::strlen(s));
    buffer1.writeByte('C');
    buffer1.writeInt8(127);
    buffer1.writeInt16(32767);
    buffer1.writeInt32(123123123);
    buffer1.writeInt64(123123123123);
    ByteBuffer buffer2(::std::move(buffer1));
    ASSERT_FALSE(buffer1.isReadable());
    ASSERT_FALSE(buffer1.isWritable());
    ASSERT_STREQ(buffer2.readBytes(::strlen(s)).c_str(), s);
    ASSERT_EQ(buffer2.readByte(), 'C');
    ASSERT_EQ(buffer2.readInt8(), 127);
    ASSERT_EQ(buffer2.readInt16(), 32767);
    ASSERT_EQ(buffer2.readInt32(), 123123123);
    ASSERT_EQ(buffer2.readInt64(), 123123123123);
    ASSERT_FALSE(buffer2.isReadable());
}

TEST(ByteBufferTest, Expansion)
{
    ByteBuffer buffer1(3);
    buffer1.writeInt32(65535);
    ASSERT_EQ(buffer1.readInt32(), 65535);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}