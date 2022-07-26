//
// Created by guang19 on 2022/7/2.
//

#include "nets/net/server/ServerBootstrap.h"

using namespace nets::net;

int main(int argc, char** argv)
{
    ServerBootstrap serverBootstrap(1, 8);
    serverBootstrap.option<ServerBootstrap>(NBackLog, 1024)
        .childHandler([](::std::shared_ptr<Channel> channel) {})
        .bind(8080)
        .launch();
    return 0;
}