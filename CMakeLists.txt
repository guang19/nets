CMAKE_MINIMUM_REQUIRED(VERSION 3.18...3.23)

PROJECT(nets LANGUAGES C CXX)

# C CXX Compiler
SET(CMAKE_C_COMPILER "/usr/bin/gcc")
SET(CMAKE_CXX_COMPILER "/usr/bin/g++")

ENABLE_TESTING()

################################################################################################
# 核心线程数，默认为可用CPU核心数的2倍，类型：size_t。
#ADD_DEFINITIONS(-DCORE_POOL_SIZE=16)
# 最大线程数，默认为可用CPU核心数的2倍，类型：size_t。
#ADD_DEFINITIONS(-DMAX_POOL_SIZE=16)
# 空闲线程可存活的最长时间，默认30000，单位：ms，类型：time_t。
ADD_DEFINITIONS(-DIDLE_KEEP_ALIVE_TIME=3000)
# TASK_QUEUE_SIZE: 任务队列大小，默认24，类型：UINT32。
ADD_DEFINITIONS(-DTASK_QUEUE_SIZE=24)
# 日志等级，默认DEBUG。可选：
# - 0：TRACE；
# - 1：DEBUG；
# - 2：INFO；
# - 3：WARN；
# - 4：ERROR；
# - 5：FATAL。
ADD_DEFINITIONS(-DLOG_LEVEL=1)
# 日志输出目标，默认为0（STDOUT）。可选：
# 0：STDOUT：标准输出；
# 1：SINGLE_FILE：输出单文件，不回滚，不计大小与时间；
# 2：DAILY_FILE：每日生成一个文件，不计大小；
# 3：ROLLING_FILE：按照文件大小回滚
ADD_DEFINITIONS(-DLOG_WRITER_TYPE=0)
# 日志文件，默认为：“/tmp/nets/nets.log”，请使用绝对路径；
ADD_DEFINITIONS(-DLOG_FILE="/tmp/nets/nets.log")
# 日志文件回滚大小，单位:MB，默认24M。（仅当LOG_WRITER为ROLLING_FILE时有效）
ADD_DEFINITIONS(-DLOG_FILE_ROLLING_SIZE=24)
################################################################################################

IF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE "Release")
ENDIF()

IF(${CMAKE_BUILD_TYPE} STREQUAL "Release")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -DNDEBUG")
ELSE()
    OPTION(BUILD_NETS_TESTS "Build nets tests" ON)
    OPTION(BUILD_NETS_EXAMPLES "Build nets examples" ON)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")
    SET(GOOGLE_TEST_INC_DIR ${PROJECT_SOURCE_DIR}/nets/third_party/googletest/include)
    SET(GOOGLE_TEST_LIB_DIR ${PROJECT_SOURCE_DIR}/nets/third_party/googletest/lib)
ENDIF()

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -g \
-Wall \
-Wextra \
-Werror \
-Wno-unused-parameter")

#set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
#set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR})
ADD_SUBDIRECTORY(nets/base)
ADD_SUBDIRECTORY(nets/net)

MESSAGE(STATUS "======CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}======")
MESSAGE(STATUS "======CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}======")