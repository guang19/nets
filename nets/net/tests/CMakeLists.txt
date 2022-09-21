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

ADD_EXECUTABLE(EpollServer EpollServer.cpp)
TARGET_LINK_LIBRARIES(EpollServer nets_net)

ADD_EXECUTABLE(EpollClient EpollClient.cpp)
TARGET_LINK_LIBRARIES(EpollClient nets_net)

ADD_EXECUTABLE(TcpServerTest TcpServerTest.cpp)
TARGET_LINK_LIBRARIES(TcpServerTest nets_net)

ADD_EXECUTABLE(TcpServerScheduleTaskTest TcpServerScheduleTaskTest.cpp)
TARGET_LINK_LIBRARIES(TcpServerScheduleTaskTest nets_net)

ADD_EXECUTABLE(TcpClientTest TcpClientTest.cpp)
TARGET_LINK_LIBRARIES(TcpClientTest nets_net)

ADD_EXECUTABLE(UdpRecipientTest UdpRecipientTest.cpp)
TARGET_LINK_LIBRARIES(UdpRecipientTest nets_net)

ADD_EXECUTABLE(UdpSenderTest UdpSenderTest.cpp)
TARGET_LINK_LIBRARIES(UdpSenderTest nets_net)