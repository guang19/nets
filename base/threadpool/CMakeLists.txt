SET(ThreadPoolHeaders ThreadPool.h BoundedBlockingQueue.hpp)
INSTALL(FILES ${ThreadPoolHeaders} DESTINATION ${PROJECT_BINARY_DIR}/include/nets/base/threadpool)

SET(ThreadPoolSrcs ThreadPool.cpp BoundedBlockingQueue.hpp)
ADD_LIBRARY(nets_base_threadpool SHARED ${ThreadPoolSrcs})
TARGET_LINK_LIBRARIES(nets_base_threadpool nets_base)
INSTALL(TARGETS nets_base_threadpool DESTINATION ${PROJECT_BINARY_DIR}/lib)