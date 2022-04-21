SET(LogHeaders Logging.h LogBuffer.h LogFormatter.hL LogWriter.h)
INSTALL(FILES ${LogHeaders} DESTINATION ${PROJECT_BINARY_DIR}/include/nets/base/log)

SET(LogSrcs Logging.cpp LogBuffer.cpp LogFormatter.cpp LogWriter.cpp)
ADD_LIBRARY(nets_base_log SHARED ${LogSrcs})
TARGET_LINK_LIBRARIES(nets_base_log nets_base)
INSTALL(TARGETS nets_base_log DESTINATION ${PROJECT_BINARY_DIR}/lib)