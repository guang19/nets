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

// @brief The encapsulation of Socket and Socket event processing

#ifndef NETS_CHANNEL_H
#define NETS_CHANNEL_H

#include <memory>
#include <unordered_map>

#include "nets/base/Noncopyable.h"
#include "nets/net/core/ChannelOption.h"

namespace nets
{
    class EventLoop;

    namespace
    {
        using EventType = Uint32Type;
        // event type
        constexpr EventType kNoneEvent = 0;
        constexpr EventType kReadEvent = 0x01;
        constexpr EventType kWriteEvent = 0x01 << 1;
        constexpr EventType kErrorEvent = 0x01 << 2;
    } // namespace

    class Channel : Noncopyable, public ::std::enable_shared_from_this<Channel>
    {
    public:
        using EventLoopRawPtr = EventLoop*;
        using ChannelOptionList = ::std::unordered_map<SockOption, ChannelOption::ValueType>;

    public:
        explicit Channel(EventLoopRawPtr eventLoop);
        virtual ~Channel() = default;

    public:
        bool registerTo();
        bool modify();
        void deregister();

    public:
        EventLoopRawPtr eventLoop() const;
        EventType events() const;
        void setEvents(EventType events);
        void addEvent(EventType event);
        bool isNoneEvent() const;
        bool hasReadEvent() const;
        bool hasWriteEvent() const;

        void setReadyEvents(EventType events);
        void addReadyEvent(EventType event);

        bool isRegistered() const;
        void setRegistered(bool registered);

    public:
        void setChannelOption(SockOption sockOption, const ChannelOption::ValueType& value);
        virtual void setBacklog(Int32Type backlog);

    public:
        virtual FdType fd() const = 0;
        void handleEvent();

    protected:
        virtual void handleReadEvent();
        virtual void handleWriteEvent();
        virtual void handleErrorEvent();

    protected:
        // channel unique identifier per EventLoop thread
        EventType events_;
        EventType readyEvents_;
        bool isRegistered_;
        EventLoopRawPtr eventLoop_;
    };
} // namespace nets

#endif // NETS_CHANNEL_H