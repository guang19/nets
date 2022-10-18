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

// @brief Bootstrap for tcp server

#include "nets/net/bootstrap/ServerBootstrap.h"

#include <unistd.h>

namespace nets
{
    const Int32Type ServerBootstrap::kDefaultNumbOfChildEventLoops = ::sysconf(_SC_NPROCESSORS_ONLN) << 1;

    ServerBootstrap::ServerBootstrap(Int32Type numOfChildEventLoops)
        : AbstractBootstrap(), childOptions_(), childHandlers_(), childInitializationCallback_(),
          childLoopGroup_(numOfChildEventLoops == 0 ? kDefaultNumbOfChildEventLoops : numOfChildEventLoops,
                          kChildEventLoopGroupName)
    {
    }

    ServerBootstrap& ServerBootstrap::childOption(SockOption sockOption, const ChannelOption::ValueType& value)
    {
        childOptions_[sockOption] = value;
        return *this;
    }

    ServerBootstrap& ServerBootstrap::childHandler(const ChannelHandlerPtr& childHandler)
    {
        if (childHandler == nullptr)
        {
            THROW_FMT(::std::invalid_argument, "ServerBootstrap sets childHandler with nullptr");
        }
        childHandlers_.push_back(childHandler);
        return *this;
    }

    ServerBootstrap& ServerBootstrap::childHandler(const ChannelInitializationCallback& childInitializationCallback)
    {
        if (childInitializationCallback == nullptr)
        {
            THROW_FMT(::std::invalid_argument, "Bootstrap sets childHandler with nullptr");
        }
        childInitializationCallback_ = childInitializationCallback;
        return *this;
    }

    ServerBootstrap& ServerBootstrap::bind(const StringType& ip, PortType port, bool ipv6)
    {
        return bind(InetSockAddress(ip, port, ipv6));
    }

    ServerBootstrap& ServerBootstrap::bind(PortType port, bool ipv6)
    {
        return bind(InetSockAddress::createAnySockAddress(port, ipv6));
    }

    ServerBootstrap& ServerBootstrap::bind(const InetSockAddress& localAddress)
    {
        mainLoopGroup_.loopEach();
        auto future = mainLoopGroup_.submit(
            [this, &localAddress]
            {
                doBind(localAddress);
            });
        future.wait();
        return *this;
    }

    void ServerBootstrap::sync()
    {
        childLoopGroup_.loopEach();
        mainLoopGroup_.syncEach();
        childLoopGroup_.syncEach();
    }

    void ServerBootstrap::shutdown()
    {
        if (!mainLoopGroup_.isShutdown())
        {
            mainLoopGroup_.shutdown();
        }
        if (!childLoopGroup_.isShutdown())
        {
            childLoopGroup_.shutdown();
        }
    }

    bool ServerBootstrap::isShutdown() const
    {
        return mainLoopGroup_.isShutdown() && childLoopGroup_.isShutdown();
    }

    void ServerBootstrap::doBind(const InetSockAddress& localAddress)
    {
        auto serverSocketChannel = ::std::make_shared<ServerSocketChannel>(mainLoopGroup_.next());
        initServerSocketChannel(serverSocketChannel);
        serverSocketChannel->bind(localAddress);
    }

    void ServerBootstrap::initServerSocketChannel(ServerSocketChannelPtr& serverSocketChannel)
    {
        serverSocketChannel->setNextEventLoopFn(
            [this]() -> EventLoopGroup::EventLoopRawPtr
            {
                return childLoopGroup_.next();
            });
        if (!channelOptions_.empty())
        {
            serverSocketChannel->setChannelOptions(channelOptions_);
            channelOptions_.clear();
        }

        if (!childOptions_.empty())
        {
            serverSocketChannel->setChildOptions(childOptions_);
            childOptions_.clear();
        }

        if (!childHandlers_.empty())
        {
            serverSocketChannel->setChildHandlers(childHandlers_);
            childHandlers_.clear();
        }

        if (childInitializationCallback_ != nullptr)
        {
            serverSocketChannel->setChildInitializationCallback(childInitializationCallback_);
            childInitializationCallback_ = nullptr;
        }

        assert(channelOptions_.empty());
        assert(childOptions_.empty());
        assert(childHandlers_.empty());
        assert(childInitializationCallback_ == nullptr);
    }
} // namespace nets