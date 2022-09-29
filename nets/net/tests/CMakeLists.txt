INCLUDE_DIRECTORIES(${GOOGLE_TEST_INC_DIR})
LINK_DIRECTORIES(${GOOGLE_TEST_LIB_DIR})

ADD_EXECUTABLE(ByteBufferTest ByteBufferTest.cpp)
TARGET_LINK_LIBRARIES(ByteBufferTest nets_net libgtest.a)

ADD_EXECUTABLE(DatagramPacketTest DatagramPacketTest.cpp)
TARGET_LINK_LIBRARIES(DatagramPacketTest nets_net libgtest.a)

ADD_EXECUTABLE(InetSockAddressTest InetSockAddressTest.cpp)
TARGET_LINK_LIBRARIES(InetSockAddressTest nets_net libgtest.a)

ADD_EXECUTABLE(SocketTest SocketTest.cpp)
TARGET_LINK_LIBRARIES(SocketTest nets_net libgtest.a)

ADD_EXECUTABLE(ChannelOptionTest ChannelOptionTest.cpp)
TARGET_LINK_LIBRARIES(ChannelOptionTest nets_net libgtest.a)

ADD_EXECUTABLE(SocketServerClientTest SocketServerClientTest.cpp)
TARGET_LINK_LIBRARIES(SocketServerClientTest nets_net libgtest.a)

ADD_EXECUTABLE(EpollServerClientTest EpollServerClientTest.cpp)
TARGET_LINK_LIBRARIES(EpollServerClientTest nets_net libgtest.a)

ADD_EXECUTABLE(TcpServerBootstrapTest TcpServerBootstrapTest.cpp)
TARGET_LINK_LIBRARIES(TcpServerBootstrapTest nets_net libgtest.a)

ADD_EXECUTABLE(UdpUnicastTest UdpUnicastTest.cpp)
TARGET_LINK_LIBRARIES(UdpUnicastTest nets_net libgtest.a)

ADD_EXECUTABLE(UdpBroadcastTest UdpBroadcastTest.cpp)
TARGET_LINK_LIBRARIES(UdpBroadcastTest nets_net libgtest.a)

ADD_EXECUTABLE(UdpMulticastTest UdpMulticastTest.cpp)
TARGET_LINK_LIBRARIES(UdpMulticastTest nets_net libgtest.a)