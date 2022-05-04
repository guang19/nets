FILE(GLOB BaseHeaders ${PROJECT_SOURCE_DIR}/nets/base/*.h)
INSTALL(FILES ${BaseHeaders} DESTINATION ${PROJECT_BINARY_DIR}/include/nets/base)
INSTALL(FILES ${BaseHeaders} DESTINATION ${PROJECT_BINARY_DIR}/include/nets/base)

FILE(GLOB ConcurrencyHeaders ${PROJECT_SOURCE_DIR}/nets/base/concurrency/*.h)
INSTALL(FILES ${ConcurrencyHeaders} DESTINATION ${PROJECT_BINARY_DIR}/include/nets/base/concurrency)

FILE(GLOB LogHeaders ${PROJECT_SOURCE_DIR}/nets/base/log/*.h)
INSTALL(FILES ${LogHeaders} DESTINATION ${PROJECT_BINARY_DIR}/include/nets/base/log)

FILE(GLOB ThreadPoolHeaders ${PROJECT_SOURCE_DIR}/nets/base/threadpool/*.h)
INSTALL(FILES ${ThreadPoolHeaders} DESTINATION ${PROJECT_BINARY_DIR}/include/nets/base/threadpool)

FILE(GLOB BaseSrcs ${PROJECT_SOURCE_DIR}/nets/base/*.cpp
        ${PROJECT_SOURCE_DIR}/nets/base/concurrency/*.cpp
        ${PROJECT_SOURCE_DIR}/nets/base/log/*.cpp
        ${PROJECT_SOURCE_DIR}/nets/base/threadpool/*.cpp)

ADD_LIBRARY(nets_base SHARED ${BaseSrcs})

TARGET_LINK_LIBRARIES(nets_base pthread)
INSTALL(TARGETS nets_base DESTINATION ${PROJECT_BINARY_DIR}/lib)

IF(BUILD_NETS_TESTS)
    ADD_SUBDIRECTORY(tests)
ENDIF()