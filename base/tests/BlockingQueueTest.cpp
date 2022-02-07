//
// Created by guang19 on 2022/1/6.
//

#include <atomic>
#include <deque>
#include <iostream>
#include <stdio.h>
#include <thread>
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
    auto blockingQueue = new nets::base::BlockingQueue<int32_t>(10);
    std::hash<std::thread::id> hasher;
    std::atomic<bool> running(true);
    for (int i = 0 ; i < 5; ++i)
    {
        std::thread([&]
                    {
                        printf("thread: %zu put...\n", hasher(std::this_thread::get_id()));
                        blockingQueue->put(i);
                        printf("thread: %zu put complete\n", hasher(std::this_thread::get_id()));
                    }).detach();
    }
    for (int i = 0 ; i < 6; ++i)
    {
        std::thread([&]
                    {
                        int32_t n;
                        blockingQueue->take(n, [&] () -> bool
                        {
                            return running;
                        });
                        if (i == 4)
                        {
                            running = false;
                        }
                        printf("thread: %zu take complete : %d\n", hasher(std::this_thread::get_id()), n);
                    }).detach();
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    return 0;
}