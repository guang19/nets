//
// Created by guang19
//

#ifndef NETS_THREAD_HELPER_H
#define NETS_THREAD_HELPER_H

#include <cstdint>
#include <pthread.h>

namespace nets
{
    namespace
    {
        // it‘s too short that the max length that unix/linux posix thread name is 16
        // constexpr ::int32_t gThreadNameMaxLength = 16;
        constexpr ::int32_t gThreadNameMaxLength = 64;
    } // namespace

    ::pid_t currentTid();
    bool isMainThread();

    void setCurrentThreadName(const char* threadName);
    const char* currentThreadName();
} // namespace nets

#endif // NETS_THREAD_HELPER_H