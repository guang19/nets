FILE(GLOB CoreHeaders ${PROJECT_SOURCE_DIR}/nets/net/core/*.h)
INSTALL(FILES ${CoreHeaders} DESTINATION ${PROJECT_BINARY_DIR}/include/nets/core)

FILE(GLOB PollerHeaders ${PROJECT_SOURCE_DIR}/nets/net/poller/*.h)
INSTALL(FILES ${PollerHeaders} DESTINATION ${PROJECT_BINARY_DIR}/include/nets/poller)

FILE(GLOB NetSrcs ${PROJECT_SOURCE_DIR}/nets/net/core/*.cpp
        ${PROJECT_SOURCE_DIR}/nets/net/poller/*.cpp)

ADD_LIBRARY(nets_net SHARED ${NetSrcs})
TARGET_LINK_LIBRARIES(nets_net nets_base)

INSTALL(TARGETS nets_net DESTINATION ${PROJECT_BINARY_DIR}/lib)

IF(BUILD_NETS_TESTS)
    ADD_SUBDIRECTORY(tests)
ENDIF()