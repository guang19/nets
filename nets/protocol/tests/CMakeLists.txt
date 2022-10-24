INCLUDE_DIRECTORIES(${GOOGLE_TEST_INC_DIR})

ADD_EXECUTABLE(HttpCommonTest HttpCommonTest.cpp)
TARGET_LINK_LIBRARIES(HttpCommonTest nets_protocol gtest_main)

ADD_EXECUTABLE(HttpServerTest HttpServerTest.cpp)
TARGET_LINK_LIBRARIES(HttpServerTest nets_protocol gtest_main)