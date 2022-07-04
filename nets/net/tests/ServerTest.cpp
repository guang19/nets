//
// Created by guang19 on 2022/7/2.
//

#include "nets/net/server/ServerBootstrap.h"

using namespace nets::net;

int main(int argc, char** argv)
{
	ServerBootstrap serverBootstrap(1, 8);
	serverBootstrap.channelHandler([](::std::shared_ptr<Channel> channel)
								   {
//										channel->pipeline().addLast();

								   }).bind(8080).launch();
	return 0;
}