FILE(GLOB BaseHeaders ${PROJECT_SOURCE_DIR}/nets/base/*.h)
INSTALL(FILES ${BaseHeaders} DESTINATION ${PROJECT_BINARY_DIR}/include/nets/base)

FILE(GLOB ExceptionHeaders ${PROJECT_SOURCE_DIR}/nets/base/exception/*.h)
INSTALL(FILES ${ConcurrencyHeaders} DESTINATION ${PROJECT_BINARY_DIR}/include/nets/base/exception)

FILE(GLOB LogHeaders ${PROJECT_SOURCE_DIR}/nets/base/log/*.h)
INSTALL(FILES ${LogHeaders} DESTINATION ${PROJECT_BINARY_DIR}/include/nets/base/log)

FILE(GLOB TimerHeaders ${PROJECT_SOURCE_DIR}/nets/base/timer/*.h)
INSTALL(FILES ${TimerHeaders} DESTINATION ${PROJECT_BINARY_DIR}/include/nets/base/timer)

FILE(GLOB BaseSrcs ${PROJECT_SOURCE_DIR}/nets/base/*.cpp
        ${PROJECT_SOURCE_DIR}/nets/base/exception/*.cpp
        ${PROJECT_SOURCE_DIR}/nets/base/log/*.cpp
        ${PROJECT_SOURCE_DIR}/nets/base/timer/*.cpp)

ADD_LIBRARY(nets_base STATIC ${BaseSrcs})

TARGET_LINK_LIBRARIES(nets_base pthread)
INSTALL(TARGETS nets_base DESTINATION ${PROJECT_BINARY_DIR}/lib)

IF (BUILD_NETS_TESTS)
    ADD_SUBDIRECTORY(tests)
ENDIF ()