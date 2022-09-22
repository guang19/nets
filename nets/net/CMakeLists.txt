FILE(GLOB BootstrapHeaders ${PROJECT_SOURCE_DIR}/nets/net/bootstrap/*.h)
INSTALL(FILES ${BootstrapHeaders} DESTINATION ${PROJECT_BINARY_DIR}/include/nets/net/bootstrap)

FILE(GLOB ChannelHeaders ${PROJECT_SOURCE_DIR}/nets/net/channel/*.h)
INSTALL(FILES ${ChannelHeaders} DESTINATION ${PROJECT_BINARY_DIR}/include/nets/net/channel)

FILE(GLOB CoreHeaders ${PROJECT_SOURCE_DIR}/nets/net/core/*.h)
INSTALL(FILES ${CoreHeaders} DESTINATION ${PROJECT_BINARY_DIR}/include/nets/net/core)

FILE(GLOB ExceptionHeaders ${PROJECT_SOURCE_DIR}/nets/net/exception/*.h)
INSTALL(FILES ${ExceptionHeaders} DESTINATION ${PROJECT_BINARY_DIR}/include/nets/net/exception)

FILE(GLOB PollerHeaders ${PROJECT_SOURCE_DIR}/nets/net/poller/*.h)
INSTALL(FILES ${PollerHeaders} DESTINATION ${PROJECT_BINARY_DIR}/include/nets/net/poller)

FILE(GLOB NetSrcs ${PROJECT_SOURCE_DIR}/nets/net/bootstrap/*.cpp
        ${PROJECT_SOURCE_DIR}/nets/net/channel/*.cpp
        ${PROJECT_SOURCE_DIR}/nets/net/core/*.cpp
        ${PROJECT_SOURCE_DIR}/nets/net/exception/*.cpp
        ${PROJECT_SOURCE_DIR}/nets/net/poller/*.cpp)

ADD_LIBRARY(nets_net STATIC ${NetSrcs})

TARGET_LINK_LIBRARIES(nets_net nets_base)
INSTALL(TARGETS nets_net DESTINATION ${PROJECT_BINARY_DIR}/lib)

IF (BUILD_NETS_TESTS)
    ADD_SUBDIRECTORY(tests)
ENDIF ()