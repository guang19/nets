INCLUDE_DIRECTORIES(${GOOGLE_TEST_INC_DIR})
LINK_DIRECTORIES(${GOOGLE_TEST_LIB_DIR})

###### Base Test
ADD_EXECUTABLE(CommonMacroTest CommonMacroTest.cpp)
TARGET_LINK_LIBRARIES(CommonMacroTest nets_base libgtest.a)

ADD_EXECUTABLE(SingletonTest SingletonTest.cpp)
TARGET_LINK_LIBRARIES(SingletonTest nets_base libgtest.a)

ADD_EXECUTABLE(TimestampTest TimestampTest.cpp)
TARGET_LINK_LIBRARIES(TimestampTest nets_base libgtest.a)

ADD_EXECUTABLE(ThreadHelperTest ThreadHelperTest.cpp)
TARGET_LINK_LIBRARIES(ThreadHelperTest nets_base libgtest.a)

ADD_EXECUTABLE(ByteStreamTest ByteStreamTest.cpp)
TARGET_LINK_LIBRARIES(ByteStreamTest nets_base libgtest.a)

###### Concurrency Test
ADD_EXECUTABLE(BlockingQueueTest BlockingQueueTest.cpp)
TARGET_LINK_LIBRARIES(BlockingQueueTest nets_base libgtest.a)

ADD_EXECUTABLE(ThreadPoolTest ThreadPoolTest.cpp)
TARGET_LINK_LIBRARIES(ThreadPoolTest nets_base libgtest.a)

###### Log Test
ADD_EXECUTABLE(LoggingTest LoggingTest.cpp)
TARGET_LINK_LIBRARIES(LoggingTest nets_base libgtest.a)