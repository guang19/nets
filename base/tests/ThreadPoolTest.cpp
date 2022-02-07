//
// Created by guang19 on 2022/1/3.
//
#include <iostream>
#include "base/threadpool/ThreadPool.h"

class F
{
    public:
        F()
        {
            std::cout << "construct\n";
        }
        ~F()
        {
            std::cout << "destruct\n";
        }
};

class Clazz
{
    public:
        Clazz()
        {
            fp = std::unique_ptr<F>(new F);
        }
        ~Clazz() = default;
    private:
        std::unique_ptr<F> fp;
};

int main(int argc, char** argv)
{
    auto threadPool = new nets::base::ThreadPool(8, 10);
    threadPool->init();
    threadPool->execute([&]{
        printf("hahaha");
    });
    return 0;
}