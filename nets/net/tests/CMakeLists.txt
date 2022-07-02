INCLUDE_DIRECTORIES(${GOOGLE_TEST_INC_DIR})
LINK_DIRECTORIES(${GOOGLE_TEST_LIB_DIR})

###### Core Test
ADD_EXECUTABLE(InetSockAddressTest InetSockAddressTest.cpp)
TARGET_LINK_LIBRARIES(InetSockAddressTest nets_net libgtest.a)

ADD_EXECUTABLE(SocketTest SocketTest.cpp)
TARGET_LINK_LIBRARIES(SocketTest nets_net libgtest.a)

ADD_EXECUTABLE(NetPackageTest NetPackageTest.cpp)
TARGET_LINK_LIBRARIES(NetPackageTest nets_net libgtest.a)

ADD_EXECUTABLE(EpollServer EpollServer.cpp)
TARGET_LINK_LIBRARIES(EpollServer nets_net)

ADD_EXECUTABLE(EpollClient EpollClient.cpp)
TARGET_LINK_LIBRARIES(EpollClient nets_net)

ADD_EXECUTABLE(ServerTest ServerTest.cpp)
TARGET_LINK_LIBRARIES(ServerTest nets_net)