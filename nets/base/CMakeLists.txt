FILE(GLOB BaseHeaders ${PROJECT_SOURCE_DIR}/nets/base/*.h)
INSTALL(FILES ${BaseHeaders} DESTINATION ${PROJECT_BINARY_DIR}/include/nets/base)

FILE(GLOB ConcurrencyHeaders ${PROJECT_SOURCE_DIR}/nets/base/concurrency/*.h)
INSTALL(FILES ${ConcurrencyHeaders} DESTINATION ${PROJECT_BINARY_DIR}/include/nets/base/concurrency)

FILE(GLOB ExceptionHeaders ${PROJECT_SOURCE_DIR}/nets/base/exception/*.h)
INSTALL(FILES ${ConcurrencyHeaders} DESTINATION ${PROJECT_BINARY_DIR}/include/nets/base/exception)

FILE(GLOB TimeHeaders ${PROJECT_SOURCE_DIR}/nets/base/time/*.h)
INSTALL(FILES ${TimeHeaders} DESTINATION ${PROJECT_BINARY_DIR}/include/nets/base/time)

FILE(GLOB LogHeaders ${PROJECT_SOURCE_DIR}/nets/base/log/*.h)
INSTALL(FILES ${LogHeaders} DESTINATION ${PROJECT_BINARY_DIR}/include/nets/base/log)

FILE(GLOB TimerHeaders ${PROJECT_SOURCE_DIR}/nets/base/timer/*.h)
INSTALL(FILES ${TimerHeaders} DESTINATION ${PROJECT_BINARY_DIR}/include/nets/base/timer)

FILE(GLOB BaseSrcs ${PROJECT_SOURCE_DIR}/nets/base/*.cpp
        ${PROJECT_SOURCE_DIR}/nets/base/concurrency/*.cpp
        ${PROJECT_SOURCE_DIR}/nets/base/exception/*.cpp
        ${PROJECT_SOURCE_DIR}/nets/base/time/*.cpp
        ${PROJECT_SOURCE_DIR}/nets/base/log/*.cpp
        ${PROJECT_SOURCE_DIR}/nets/base/timer/*.cpp)

ADD_LIBRARY(nets_base STATIC ${BaseSrcs})

TARGET_LINK_LIBRARIES(nets_base pthread)
INSTALL(TARGETS nets_base DESTINATION ${PROJECT_BINARY_DIR}/lib)

IF (BUILD_NETS_TESTS)
    ADD_LIBRARY(nets_base_shared SHARED ${BaseSrcs})
    TARGET_LINK_LIBRARIES(nets_base_shared pthread)
    ADD_SUBDIRECTORY(tests)
ENDIF ()