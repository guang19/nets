//
// Created by guang19
//

#ifndef NETS_BASE_THREAD_HELPER_H
#define NETS_BASE_THREAD_HELPER_H

#include <cstdint>
#include <pthread.h>

namespace nets::base
{
    namespace
    {
        // it‘s too short that the max length that unix/linux posix thread name is 16
        // constexpr ::uint32_t ThreadNameMaxLength = 16;
        constexpr ::uint32_t ThreadNameMaxLength = 64;
    } // namespace

    ::pid_t currentTid();
    bool isMainThread();

    void setCurrentThreadName(const char* threadName);
    const char* currentThreadName();
} // namespace nets::base

#endif // NETS_BASE_THREAD_HELPER_H