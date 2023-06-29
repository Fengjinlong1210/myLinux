#include <iostream>
#include <unistd.h>
#include <string>
#include <pthread.h>
#include "Thread.hpp"
#include "LockGuard.hpp"
using namespace std;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int ticket_num = 1000;

void *pthread_func(void *args)
{
    char *name = static_cast<char *>(args);
    while (1)
    {
        {
            LockGuard lock(&mutex);
            if (ticket_num > 0)
            {
                cout << "我是" << name << ", 我抢到票了, 剩余" << ticket_num << endl;
                ticket_num--;
                // usleep(2000);
                // 模拟抢票后的行为
                // 如果不休眠, 某个线程在进入临界区后, 时间片到了, 切换了其他的线程被调度
                // 这个新线程把变量值改了, 当原线程被切回时, 仍然保留的是之前的值
                // 就导致新线程的更改失效, 变量值也有可能被减到负数
            }
            else
            {
                break;
            }
        }
    }
}

int main()
{
    Thread t1(1, pthread_func, (void*)"一号线程");
    Thread t2(2, pthread_func, (void*)"二号线程");
    Thread t3(3, pthread_func, (void*)"三号线程");
    Thread t4(4, pthread_func, (void*)"四号线程");

    t1.run();
    t2.run();
    t3.run();
    t4.run();
    
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    return 0;
}

// void *thread_func(void *args)
// {
//     string name = static_cast<const char *>(args);
//     while (1)
//     {
//         cout << "我是" << name << endl;
//         sleep(1);
//     }
// }

// int main()
// {
//     char buffer[64];
//     snprintf(buffer, sizeof(buffer), "1号线程");
//     Thread t1(1, thread_func, buffer);
//     t1.run();
//     t1.join();
//     return 0;
// }


