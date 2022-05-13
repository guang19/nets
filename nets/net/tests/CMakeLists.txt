INCLUDE_DIRECTORIES(${GOOGLE_TEST_INC_DIR})
LINK_DIRECTORIES(${GOOGLE_TEST_LIB_DIR})

###### Core Test
ADD_EXECUTABLE(InetUtilTest InetUtilTest.cpp)
TARGET_LINK_LIBRARIES(InetUtilTest nets_net libgtest.a)

ADD_EXECUTABLE(NetPackageTest NetPackageTest.cpp)
TARGET_LINK_LIBRARIES(NetPackageTest nets_net libgtest.a)

ADD_EXECUTABLE(SocketTest SocketTest.cpp)
TARGET_LINK_LIBRARIES(SocketTest nets_net libgtest.a)