INCLUDE_DIRECTORIES(${GOOGLE_TEST_INC_DIR})
LINK_DIRECTORIES(${GOOGLE_TEST_LIB_DIR})

###### Core Test
ADD_EXECUTABLE(InetSockAddressTest InetSockAddressTest.cpp)
TARGET_LINK_LIBRARIES(InetSockAddressTest nets_net_static libgtest.a)

ADD_EXECUTABLE(SocketTest SocketTest.cpp)
TARGET_LINK_LIBRARIES(SocketTest nets_net_static libgtest.a)

ADD_EXECUTABLE(NetPackageTest NetPackageTest.cpp)
TARGET_LINK_LIBRARIES(NetPackageTest nets_net_static libgtest.a)