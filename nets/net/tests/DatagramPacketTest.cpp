//
// Created by guang19
//

#include <gtest/gtest.h>

#include "nets/net/core/DatagramPacket.h"

using namespace nets::net;

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