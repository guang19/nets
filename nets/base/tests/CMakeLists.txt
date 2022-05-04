INCLUDE_DIRECTORIES(${GOOGLE_TEST_INC_DIR})
LINK_DIRECTORIES(${GOOGLE_TEST_LIB_DIR})

###### Base Test
ADD_EXECUTABLE(CommonMacroTest CommonMacroTest.cpp)
TARGET_LINK_LIBRARIES(CommonMacroTest nets_base libgtest.a)

ADD_EXECUTABLE(SingletonTest SingletonTest.cpp)
TARGET_LINK_LIBRARIES(SingletonTest nets_base libgtest.a)

ADD_EXECUTABLE(TimestampTest TimestampTest.cpp)
TARGET_LINK_LIBRARIES(TimestampTest nets_base libgtest.a)

ADD_EXECUTABLE(ThreadTest ThreadTest.cpp)
TARGET_LINK_LIBRARIES(ThreadTest nets_base libgtest.a)

###### Concurrency Test
ADD_EXECUTABLE(ConcurrencyTest ConcurrencyTest.cpp)
TARGET_LINK_LIBRARIES(ConcurrencyTest nets_base libgtest.a)

ADD_EXECUTABLE(BlockingQueueTest BlockingQueueTest.cpp)
TARGET_LINK_LIBRARIES(BlockingQueueTest nets_base libgtest.a)

ADD_EXECUTABLE(ThreadPoolTest ThreadPoolTest.cpp)
TARGET_LINK_LIBRARIES(ThreadPoolTest nets_base libgtest.a)

###### Log Test
ADD_EXECUTABLE(LogTest LoggingTest.cpp)
TARGET_LINK_LIBRARIES(LogTest nets_base libgtest.a)

ADD_EXECUTABLE(LogBufferTest LogBufferTest.cpp)
TARGET_LINK_LIBRARIES(LogBufferTest nets_base libgtest.a)