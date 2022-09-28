//
// Created by guang19
//

#include "nets/base/ThreadHelper.h"

#include <sys/syscall.h>
#include <unistd.h>

#include "nets/base/CommonMacro.h"

namespace nets::base
{
    namespace
    {
        constexpr const char* const gMainThreadName = "Main";
        __thread ::pid_t gCacheTid = 0;
        __thread char gThreadName[gThreadNameMaxLength] = "unnamed";
    } // namespace

    ::pid_t getTid()
    {
        return static_cast<pid_t>(::syscall(SYS_gettid));
    }

    ::pid_t currentTid()
    {
        if (gCacheTid <= 0)
        {
            gCacheTid = getTid();
        }
        return gCacheTid;
    }

    bool isMainThread()
    {
        return ::getpid() == currentTid();
    }

    void afterFork()
    {
        gCacheTid = 0;
        MEMZERO(gThreadName, gThreadNameMaxLength);
        ::memcpy(gThreadName, gMainThreadName, ::strlen(gMainThreadName));
    }

    struct ThreadInitializer
    {
        ThreadInitializer()
        {
            currentTid();
            MEMZERO(gThreadName, gThreadNameMaxLength);
            ::memcpy(gThreadName, gMainThreadName, ::strlen(gMainThreadName));
            ::pthread_atfork(nullptr, nullptr, &afterFork);
        }
    };

    // init main thread
    ThreadInitializer threadInitializer {};

    void setCurrentThreadName(const char* threadName)
    {
        MEMZERO(gThreadName, gThreadNameMaxLength);
        ::memcpy(gThreadName, threadName, ::strlen(threadName));
    }

    const char* currentThreadName()
    {
        return gThreadName;
    }
} // namespace nets::base
