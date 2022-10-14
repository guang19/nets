INCLUDE_DIRECTORIES(${GOOGLE_TEST_INC_DIR})

ADD_EXECUTABLE(ByteBufferTest ByteBufferTest.cpp)
TARGET_LINK_LIBRARIES(ByteBufferTest nets_net gtest_main)

ADD_EXECUTABLE(DatagramPacketTest DatagramPacketTest.cpp)
TARGET_LINK_LIBRARIES(DatagramPacketTest nets_net gtest_main)

ADD_EXECUTABLE(InetSockAddressTest InetSockAddressTest.cpp)
TARGET_LINK_LIBRARIES(InetSockAddressTest nets_net gtest_main)

ADD_EXECUTABLE(SocketTest SocketTest.cpp)
TARGET_LINK_LIBRARIES(SocketTest nets_net gtest_main)

ADD_EXECUTABLE(ChannelOptionTest ChannelOptionTest.cpp)
TARGET_LINK_LIBRARIES(ChannelOptionTest nets_net gtest_main)

ADD_EXECUTABLE(ChannelHandlerPipelineTest ChannelHandlerPipelineTest.cpp)
TARGET_LINK_LIBRARIES(ChannelHandlerPipelineTest nets_net gtest_main)

ADD_EXECUTABLE(SocketServerClientTest SocketServerClientTest.cpp)
TARGET_LINK_LIBRARIES(SocketServerClientTest nets_net gtest_main)

ADD_EXECUTABLE(EpollServerClientTest EpollServerClientTest.cpp)
TARGET_LINK_LIBRARIES(EpollServerClientTest nets_net gtest_main)

ADD_EXECUTABLE(TcpServerBootstrapTest TcpServerBootstrapTest.cpp)
TARGET_LINK_LIBRARIES(TcpServerBootstrapTest nets_net gtest_main)

ADD_EXECUTABLE(ScheduleTaskTest ScheduleTaskTest.cpp)
TARGET_LINK_LIBRARIES(ScheduleTaskTest nets_net gtest_main)

ADD_EXECUTABLE(UdpUnicastTest UdpUnicastTest.cpp)
TARGET_LINK_LIBRARIES(UdpUnicastTest nets_net gtest_main)

ADD_EXECUTABLE(UdpBroadcastTest UdpBroadcastTest.cpp)
TARGET_LINK_LIBRARIES(UdpBroadcastTest nets_net gtest_main)

ADD_EXECUTABLE(UdpMulticastTest UdpMulticastTest.cpp)
TARGET_LINK_LIBRARIES(UdpMulticastTest nets_net gtest_main)