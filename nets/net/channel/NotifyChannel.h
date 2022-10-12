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

#ifndef NETS_NOTIFY_CHANNEL_H
#define NETS_NOTIFY_CHANNEL_H

#include "nets/net/channel/Channel.h"

namespace nets
{
    class NotifyChannel : public Channel
    {
    public:
        explicit NotifyChannel(EventLoopRawPtr eventLoop);
        ~NotifyChannel() override;

    public:
        FdType fd() const override;
        void notify() const;

    protected:
        void handleReadEvent() override;

    private:
        FdType eventFd_;
    };
} // namespace nets

#endif // NETS_NOTIFY_CHANNEL_H