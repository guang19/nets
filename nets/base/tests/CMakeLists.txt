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

ADD_EXECUTABLE(TimerTest TimerTest.cpp)
TARGET_LINK_LIBRARIES(TimerTest nets_base_shared libgtest.a)

ADD_EXECUTABLE(TimerManagerTest TimerManagerTest.cpp)
TARGET_LINK_LIBRARIES(TimerManagerTest nets_base_shared libgtest.a)

ADD_EXECUTABLE(StackBufferTest StackBufferTest.cpp)
TARGET_LINK_LIBRARIES(StackBufferTest nets_base_shared libgtest.a)

ADD_EXECUTABLE(LogBufferStreamTest LogBufferStreamTest.cpp)
TARGET_LINK_LIBRARIES(LogBufferStreamTest nets_base_shared libgtest.a)

ADD_EXECUTABLE(LoggerTest LoggerTest.cpp)
TARGET_LINK_LIBRARIES(LoggerTest nets_base_shared libgtest.a)

ADD_EXECUTABLE(ConfigTest ConfigTest.cpp)
TARGET_LINK_LIBRARIES(ConfigTest nets_base_shared libgtest.a)