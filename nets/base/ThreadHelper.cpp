// MIT License
//
// Copyright (c) 2022 guang19
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

// @brief Operations on thread

#include "nets/base/ThreadHelper.h"

#include <sys/syscall.h>
#include <unistd.h>

#include "nets/base/Common.h"

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
