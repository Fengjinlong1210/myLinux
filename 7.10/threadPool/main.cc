#include "LockGuard.hpp"
#include "thread_pool.hpp"
#include "Thread.hpp"
#include "task.hpp"
#include <iostream>
#include <memory>
using namespace std;
string opt = "+-*/%";

int main()
{
    // unique_ptr<threadPool<Task>> pool_ptr(new threadPool<Task>(8));
    // pool_ptr->init();

    // pool_ptr->startRunning();
    // pool_ptr->check();//    检查线程是否运行

    auto instance = threadPool<Task>::get_instance();

    srand((unsigned int)time(nullptr));
    while(1)
    {
        int num1 = rand() % 100 + 1;
        int num2 = rand() % 100 + 1;
        int n = rand() % opt.size();
        char op = opt[n];
        //cout << "num ready" << endl;
        Task t(num1, num2, op);
        
        //pool_ptr->pushTask(t);
        instance->pushTask(t);
        sleep(1);
    }
    return 0;
}