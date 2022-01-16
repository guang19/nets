SET(BaseHeaders copyable.h noncopyable.h Singleton.h V.h)
INSTALL(FILES ${BaseHeaders} DESTINATION ${PROJECT_BINARY_DIR}/include/nets/base)

SET(BaseSrcs V.cpp)
ADD_LIBRARY(nets_base SHARED ${BaseSrcs})
TARGET_LINK_LIBRARIES(nets_base pthread)
INSTALL(TARGETS nets_base DESTINATION ${PROJECT_BINARY_DIR}/lib)

ADD_SUBDIRECTORY(threadpool)
IF(BUILD_NETS_TESTS)
    ADD_SUBDIRECTORY(tests)
ENDIF()