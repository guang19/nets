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

// @brief This Channel sends a message to the file descriptor managed by EventLoop
// to wake up EventLoop

#include "nets/net/channel/NotifyChannel.h"

#include <stdexcept>
#include <sys/eventfd.h>

#include "nets/base/log/Logger.h"

namespace nets
{
    NotifyChannel::NotifyChannel(EventLoopRawPtr eventLoop)
        : Channel(eventLoop), eventFd_(::eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK))
    {
        if (eventFd_ < 0)
        {
            THROW_FMT(::std::runtime_error, "NotifyChannel create eventFd failed");
        }
    }

    NotifyChannel::~NotifyChannel()
    {
        socket::closeFd(eventFd_);
    }

    FdType NotifyChannel::fd() const
    {
        return eventFd_;
    }

    void NotifyChannel::notify() const
    {
        Int64Type i = 1L;
        SSizeType n = socket::write(eventFd_, &i, sizeof(i));
        if (n != sizeof(i))
        {
            NETS_SYSTEM_LOG_ERROR << "NotifyChannel notify failed";
        }
    }

    void NotifyChannel::handleReadEvent()
    {
        Int64Type i = 0L;
        SSizeType n = socket::read(eventFd_, &i, sizeof(i));
        if (n != sizeof(i))
        {
            NETS_SYSTEM_LOG_ERROR << "NotifyChannel read failed";
        }
    }
} // namespace nets