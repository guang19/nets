INCLUDE_DIRECTORIES(${GOOGLE_TEST_INC_DIR})

ADD_EXECUTABLE(HttpServerTest HttpServerTest.cpp)
TARGET_LINK_LIBRARIES(HttpServerTest nets_protocol gtest_main)