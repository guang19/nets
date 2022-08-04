INCLUDE_DIRECTORIES(${GOOGLE_TEST_INC_DIR})
LINK_DIRECTORIES(${GOOGLE_TEST_LIB_DIR})

ADD_EXECUTABLE(SingletonTest SingletonTest.cpp)
TARGET_LINK_LIBRARIES(SingletonTest nets_base libgtest.a)

ADD_EXECUTABLE(TimestampTest TimestampTest.cpp)
TARGET_LINK_LIBRARIES(TimestampTest nets_base_shared libgtest.a)

ADD_EXECUTABLE(ThreadHelperTest ThreadHelperTest.cpp)
TARGET_LINK_LIBRARIES(ThreadHelperTest nets_base_shared libgtest.a)

ADD_EXECUTABLE(BlockingQueueTest BlockingQueueTest.cpp)
TARGET_LINK_LIBRARIES(BlockingQueueTest nets_base_shared libgtest.a)

ADD_EXECUTABLE(ThreadPoolTest ThreadPoolTest.cpp)
TARGET_LINK_LIBRARIES(ThreadPoolTest nets_base_shared libgtest.a)

ADD_EXECUTABLE(LogBufferStreamTest LogBufferStreamTest.cpp)
TARGET_LINK_LIBRARIES(LogBufferStreamTest nets_base_shared libgtest.a)

ADD_EXECUTABLE(LoggingTest LoggingTest.cpp)
TARGET_LINK_LIBRARIES(LoggingTest nets_base_shared libgtest.a)

ADD_EXECUTABLE(ExceptionTest ExceptionTest.cpp)
TARGET_LINK_LIBRARIES(ExceptionTest nets_base_shared libgtest.a)