//
// Created by guang19
//

#include "nets/net/channel/NotifyChannel.h"

#include <stdexcept>
#include <sys/eventfd.h>

#include "nets/base/log/Logging.h"

namespace nets::net
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
        ::int64_t i = 1L;
        SSizeType n = socket::write(eventFd_, &i, sizeof(i));
        if (n != sizeof(i))
        {
            LOGS_ERROR << "NotifyChannel notify failed";
        }
    }

    void NotifyChannel::handleReadEvent()
    {
        ::int64_t i = 0L;
        SSizeType n = socket::read(eventFd_, &i, sizeof(i));
        if (n != sizeof(i))
        {
            LOGS_ERROR << "NotifyChannel read failed";
        }
    }
} // namespace nets::net