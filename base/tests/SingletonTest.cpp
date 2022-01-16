//
// Created by guang19 on 2022/1/4.
//

#include <iostream>
#include <string>
#include <thread>
#include "base/Singleton.h"

class Clazz
{
    public:
        Clazz()
        {
            std::cout << "Clazz Construct " << std::endl;
        }
        Clazz(const std::string& name) : name_(name)
        {
            std::cout << "Clazz Construct " << name_ << std::endl;
        }

        ~Clazz()
        {
            std::cout << "Clazz Destruct " << name_ << std::endl;
        }

        void print()
        {
            std::cout << "Clazz = " << name_ << " thread id: " << std::this_thread::get_id() << " " << this <<  std::endl;
        }
    private:
        std::string name_;
};

int main(int argc, char** argv)
{
    Clazz* cp = nullptr;
    std::thread t1([&] {
        cp = nets::base::Singleton<Clazz>::getInstance("cp");
        cp->print();
    });

    std::thread t2([&] {
        cp = nets::base::Singleton<Clazz>::getInstance("cp2");
        cp->print();
    });
    t1.join();
    t2.join();
    return 0;
}
