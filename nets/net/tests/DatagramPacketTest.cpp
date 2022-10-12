// MIT License
//
// Copyright (c) 2022 guang19
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

// @brief Test for DatagramPacket

#include <gtest/gtest.h>

#include "nets/net/core/DatagramPacket.h"

using namespace nets;

TEST(DatagramPacketTest, BasicUse)
{
    const char* str = "Hello World";
    DatagramPacket packet1(ByteBuffer(str, ::strlen(str)), InetSockAddress::createAnySockAddress(8080));
    ::printf("packet1:%s\t%s\n", packet1.data(), packet1.recipient().toString().data());
    DatagramPacket packet2(packet1);
    ::printf("packet2:%s\t%s\n", packet2.data(), packet2.recipient().toString().data());
    DatagramPacket packet3(::std::move(packet2));
    ::printf("packet2:%s\t%s\n", packet2.data(), packet2.recipient().toString().data());
    ::printf("packet3:%s\t%s\n", packet3.data(), packet3.recipient().toString().data());
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}