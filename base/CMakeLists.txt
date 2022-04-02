SET(BaseHeaders Copyable.h Noncopyable.h Singleton.h)
INSTALL(FILES ${BaseHeaders} DESTINATION ${PROJECT_BINARY_DIR}/include/nets/base)

SET(BaseSrcs Copyable.h Noncopyable.h Singleton.h)
ADD_LIBRARY(nets_base SHARED ${BaseSrcs})
TARGET_LINK_LIBRARIES(nets_base pthread)
SET_TARGET_PROPERTIES(nets_base PROPERTIES LINKER_LANGUAGE CXX)
INSTALL(TARGETS nets_base DESTINATION ${PROJECT_BINARY_DIR}/lib)

ADD_SUBDIRECTORY(log)
ADD_SUBDIRECTORY(threadpool)

IF(BUILD_NETS_TESTS)
    ADD_SUBDIRECTORY(tests)
ENDIF()