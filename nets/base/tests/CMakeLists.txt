INCLUDE_DIRECTORIES(${GOOGLE_TEST_INC_DIR})

ADD_EXECUTABLE(StringUtilsTest StringUtilsTest.cpp)
TARGET_LINK_LIBRARIES(StringUtilsTest nets_base gtest_main)

ADD_EXECUTABLE(SingletonTest SingletonTest.cpp)
TARGET_LINK_LIBRARIES(SingletonTest nets_base gtest_main)

ADD_EXECUTABLE(TimestampTest TimestampTest.cpp)
TARGET_LINK_LIBRARIES(TimestampTest nets_base gtest_main)

ADD_EXECUTABLE(ThreadHelperTest ThreadHelperTest.cpp)
TARGET_LINK_LIBRARIES(ThreadHelperTest nets_base gtest_main)

ADD_EXECUTABLE(BoundedBlockingQueueTest BoundedBlockingQueueTest.cpp)
TARGET_LINK_LIBRARIES(BoundedBlockingQueueTest nets_base gtest_main)

ADD_EXECUTABLE(ThreadPoolTest ThreadPoolTest.cpp)
TARGET_LINK_LIBRARIES(ThreadPoolTest nets_base gtest_main)

ADD_EXECUTABLE(TimerTest TimerTest.cpp)
TARGET_LINK_LIBRARIES(TimerTest nets_base gtest_main)

ADD_EXECUTABLE(TimerManagerTest TimerManagerTest.cpp)
TARGET_LINK_LIBRARIES(TimerManagerTest nets_base gtest_main)

ADD_EXECUTABLE(StackBufferTest StackBufferTest.cpp)
TARGET_LINK_LIBRARIES(StackBufferTest nets_base gtest_main)

ADD_EXECUTABLE(LogBufferStreamTest LogBufferStreamTest.cpp)
TARGET_LINK_LIBRARIES(LogBufferStreamTest nets_base gtest_main)

ADD_EXECUTABLE(LoggerTest LoggerTest.cpp)
TARGET_LINK_LIBRARIES(LoggerTest nets_base gtest_main)