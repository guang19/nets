//
// Created by guang19
//

#include "nets/base/ThreadHelper.h"

#include <sys/syscall.h>
#include <unistd.h>

#include "nets/base/CommonMacro.h"

namespace nets
{
    namespace
    {
        constexpr const char* const kMainThreadName = "Main";
        __thread ::pid_t tCacheTid = 0;
        __thread char tThreadName[kThreadNameMaxLength] = "unnamed";
    } // namespace

    ::pid_t getTid()
    {
        return static_cast<pid_t>(::syscall(SYS_gettid));
    }

    ::pid_t currentTid()
    {
        if (tCacheTid <= 0)
        {
            tCacheTid = getTid();
        }
        return tCacheTid;
    }

    bool isMainThread()
    {
        return ::getpid() == currentTid();
    }

    void afterFork()
    {
        tCacheTid = 0;
        MEMZERO(tThreadName, kThreadNameMaxLength);
        ::memcpy(tThreadName, kMainThreadName, ::strlen(kMainThreadName));
    }

    struct ThreadInitializer
    {
        ThreadInitializer()
        {
            currentTid();
            MEMZERO(tThreadName, kThreadNameMaxLength);
            ::memcpy(tThreadName, kMainThreadName, ::strlen(kMainThreadName));
            ::pthread_atfork(nullptr, nullptr, &afterFork);
        }
    };

    // init main thread
    ThreadInitializer threadInitializer {};

    void setCurrentThreadName(const char* threadName)
    {
        MEMZERO(tThreadName, kThreadNameMaxLength);
        ::memcpy(tThreadName, threadName, ::strlen(threadName));
    }

    const char* currentThreadName()
    {
        return tThreadName;
    }
} // namespace nets
