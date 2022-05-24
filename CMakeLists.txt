CMAKE_MINIMUM_REQUIRED(VERSION 3.18...3.23)

PROJECT(nets LANGUAGES C CXX)

# C CXX Compiler
SET(CMAKE_C_COMPILER "/usr/bin/gcc")
SET(CMAKE_CXX_COMPILER "/usr/bin/g++")

ENABLE_TESTING()

################################################################################################
# the numbers of core threads
#ADD_DEFINITIONS(-DCORE_POOL_SIZE=16)
# the maximum numbers of threads that the thread pool can hold
#ADD_DEFINITIONS(-DMAX_POOL_SIZE=16)
#  time that idle threads can survive, unit: ms
ADD_DEFINITIONS(-DIDLE_KEEP_ALIVE_TIME=3000)
# task queue size
ADD_DEFINITIONS(-DTASK_QUEUE_SIZE=24)
# log level,default DEBUG. option：
# - 0：TRACE
# - 1：DEBUG
# - 2：INFO
# - 3：WARN
# - 4：ERROR
# - 5：FATAL
ADD_DEFINITIONS(-DLOG_LEVEL=1)
# log writer type,default 0(stdout).option：
# 0：STDOUT:stdout.
# 1：SINGLE_FILE:out to single file,no rollback,regardless of size and time.
# 2：DAILY_FILE:generate file daily.
# 3：ROLLING_FILE:rollback by file size.
ADD_DEFINITIONS(-DLOG_WRITER_TYPE=0)
# log file, default "/tmp/nets/nets.log", must be absolute path.
ADD_DEFINITIONS(-DLOG_FILE="/tmp/nets/nets.log")
# log file rollback size, unit:MB, default 24. (ROLLING_FILE takes effect only if LOG_WRITER_TYPE is 0)
ADD_DEFINITIONS(-DLOG_FILE_ROLLING_SIZE=24)
################################################################################################

IF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE "Release")
ENDIF()

IF(${CMAKE_BUILD_TYPE} STREQUAL "Release")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -DNDEBUG")
ELSE()
    OPTION(BUILD_NETS_TESTS "Build nets tests" ON)
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