INCLUDE_DIRECTORIES(${GOOGLE_TEST_INC_DIR})
LINK_DIRECTORIES(${GOOGLE_TEST_LIB_DIR})

ADD_EXECUTABLE(SingletonTest SingletonTest.cpp)
TARGET_LINK_LIBRARIES(SingletonTest nets_base libgtest.a)

ADD_EXECUTABLE(BlockingQueueTest BlockingQueueTest.cpp)
TARGET_LINK_LIBRARIES(BlockingQueueTest nets_base_threadpool libgtest.a)

ADD_EXECUTABLE(ThreadPoolTest ThreadPoolTest.cpp)
TARGET_LINK_LIBRARIES(ThreadPoolTest nets_base_threadpool libgtest.a)

ADD_EXECUTABLE(LogTest LoggingTest.cpp)
TARGET_LINK_LIBRARIES(LogTest nets_base_log libgtest.a)

ADD_EXECUTABLE(LogBufferTest LogBufferTest.cpp)
TARGET_LINK_LIBRARIES(LogBufferTest nets_base_log libgtest.a)