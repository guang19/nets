FILE(GLOB BaseHeaders ${PROJECT_SOURCE_DIR}/nets/base/*.h)
INSTALL(FILES ${BaseHeaders} DESTINATION ${PROJECT_BINARY_DIR}/include/nets/base)

FILE(GLOB ConcurrencyHeaders ${PROJECT_SOURCE_DIR}/nets/base/concurrency/*.h)
INSTALL(FILES ${ConcurrencyHeaders} DESTINATION ${PROJECT_BINARY_DIR}/include/nets/base/concurrency)

FILE(GLOB LogHeaders ${PROJECT_SOURCE_DIR}/nets/base/log/*.h)
INSTALL(FILES ${LogHeaders} DESTINATION ${PROJECT_BINARY_DIR}/include/nets/base/log)

FILE(GLOB BaseSrcs ${PROJECT_SOURCE_DIR}/nets/base/*.cpp
        ${PROJECT_SOURCE_DIR}/nets/base/concurrency/*.cpp
        ${PROJECT_SOURCE_DIR}/nets/base/log/*.cpp)

ADD_LIBRARY(nets_base_shared SHARED ${BaseSrcs})
ADD_LIBRARY(nets_base_static STATIC ${BaseSrcs})

TARGET_LINK_LIBRARIES(nets_base_shared pthread)
TARGET_LINK_LIBRARIES(nets_base_static pthread)

INSTALL(TARGETS nets_base_static DESTINATION ${PROJECT_BINARY_DIR}/lib)

IF(BUILD_NETS_TESTS)
    ADD_SUBDIRECTORY(tests)
ENDIF()