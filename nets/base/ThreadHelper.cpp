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
        constexpr const char* const MainThreadName = "Main";
        __thread ::pid_t CacheTid = 0;
        __thread char ThreadName[ThreadNameMaxLength] = "unnamed";
    } // namespace

    ::pid_t getTid()
    {
        return static_cast<pid_t>(::syscall(SYS_gettid));
    }

    ::pid_t currentTid()
    {
        if (CacheTid <= 0)
        {
            CacheTid = getTid();
        }
        return CacheTid;
    }

    bool isMainThread()
    {
        return ::getpid() == currentTid();
    }

    void afterFork()
    {
        CacheTid = 0;
        MEMZERO(ThreadName, ThreadNameMaxLength);
        ::memcpy(ThreadName, MainThreadName, ::strlen(MainThreadName));
    }

    struct ThreadInitializer
    {
        ThreadInitializer()
        {
            currentTid();
            MEMZERO(ThreadName, ThreadNameMaxLength);
            ::memcpy(ThreadName, MainThreadName, ::strlen(MainThreadName));
            ::pthread_atfork(nullptr, nullptr, &afterFork);
        }
    };

    // init main thread
    ThreadInitializer threadInitializer {};

    void setCurrentThreadName(const char* threadName)
    {
        MEMZERO(ThreadName, ThreadNameMaxLength);
        ::memcpy(ThreadName, threadName, ::strlen(threadName));
    }

    const char* currentThreadName()
    {
        return ThreadName;
    }
} // namespace nets::base
