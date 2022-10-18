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

// @brief Bootstrap for tcp and udp

#ifndef NETS_ABSTRACT_BOOTSTRAP_H
#define NETS_ABSTRACT_BOOTSTRAP_H

#include "nets/base/exception/SegmentationFaultException.h"
#include "nets/base/SignalHandler.h"
#include "nets/net/core/ChannelOption.h"
#include "nets/net/core/EventLoopGroup.h"
#include "nets/base/ThreadHelper.h"

namespace nets
{
    template <class B>
    class AbstractBootstrap : Noncopyable
    {
    public:
        using ChannelOptionList = Channel::ChannelOptionList;

    public:
        AbstractBootstrap() : channelOptions_(), mainLoopGroup_(kNumbOfMainEventLoops, kMainEventLoopGroupName)
        {
            SignalHandler::initSignalHandler(::std::bind(&AbstractBootstrap<B>::handleSignal, this, ::std::placeholders::_1,
                                                         ::std::placeholders::_2, ::std::placeholders::_3));
        }

        virtual ~AbstractBootstrap() = default;

    public:
        B& option(SockOption sockOption, const ChannelOption::ValueType& value)
        {
            channelOptions_[sockOption] = value;
            return static_cast<B&>(*this);
        }

        virtual void shutdown()
        {
            if (!mainLoopGroup_.isShutdown())
            {
                mainLoopGroup_.shutdown();
            }
        }

        virtual bool isShutdown() const
        {
            return mainLoopGroup_.isShutdown();
        }

    private:
        void handleSignal(SignalHandler::SignoType signo, SignalHandler::SigInfo* info, void* context)
        {
            UNUSED(context);
            switch (signo)
            {
                case SIGPIPE:
                    break;
                case SIGHUP:
                case SIGINT:
                case SIGQUIT:
                case SIGTERM:
                {
                    shutdown();
                    break;
                }
                case SIGSEGV:
                {
                    NETS_SYSTEM_LOG_ERROR << "handleSignal SIGSEGV backtrace info:\n" << stackTrace();
                    THROW_FMT(SegmentationFaultException, "segmentation fault,errno=%d,si_uid=%d,si_pid=%d", info->si_errno,
                              info->si_uid, info->si_pid);
                    break;
                }
            }
        }

    protected:
        ChannelOptionList channelOptions_;
        EventLoopGroup mainLoopGroup_;

    private:
        static constexpr Int32Type kNumbOfMainEventLoops = 1;
        static constexpr char kMainEventLoopGroupName[] = "MainLoopGroup";
    };
} // namespace nets

#endif // NETS_ABSTRACT_BOOTSTRAP_H