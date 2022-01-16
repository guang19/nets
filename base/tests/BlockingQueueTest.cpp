//
// Created by guang19 on 2022/1/6.
//

#include <deque>
#include <iostream>
#include <stdio.h>
#include <thread>
#include "base/Singleton.h"
#include "base/threadpool/BlockingQueue.h"


class F
{
    public:
        F()
        {
            std::cout << "construct\n";
        }

        F(int _a) : a(_a)
        {
            std::cout << "construct " << a << '\n';
        }

        F(const F& rhs)
        {
            std::cout << "copy construct\n";
        }

        F(F&& rhs)
        {
            std::cout << "moving copy construct\n";
        }

        ~F()
        {
            std::cout << "destruct\n";
        }

        int& getA()
        {
            return a;
        }

    private:
        int a;
};

void setIntRef(int& n)
{
    n = 5;
}

F getF()
{
    F f;
    return f;
}


int main(int argc, char** argv)
{
//    auto blockingQueue =
//            nets::base::Singleton<nets::base::BlockingQueue<int32_t>>::getInstance(10);
//    std::hash<std::thread::id> hasher;
//    for (int i = 0 ; i < 21; ++i)
//    {
//        std::thread([&]
//                    {
//                        printf("thread: %zu put...\n", hasher(std::this_thread::get_id()));
//                        blockingQueue->put(i);
//                        printf("thread: %zu put complete\n", hasher(std::this_thread::get_id()));
//                    }).detach();
//    }
//    for (int i = 0 ; i < 10; ++i)
//    {
//        std::thread([&]
//                    {
//                        printf("%d\n", blockingQueue->take());
//                        printf("thread: %zu take complete\n", hasher(std::this_thread::get_id()));
//                    }).detach();
//    }
//    std::this_thread::sleep_for(std::chrono::milliseconds(3000));
//    std::deque<F> fdq;
//    fdq.emplace_back(1);
//    fdq.emplace_back(2);
//    fdq.emplace_back(3);
//
//    std::cout << "begin...\n";
//    F f = std::move(fdq.front());
//    std::cout << fdq.front().getA() << '\n';
//    std::cout << "end\n";
    F f(3);
    std::cout << f.getA() << '\n';
    setIntRef(f.getA());
    std::cout << f.getA() << '\n';
    return 0;
}