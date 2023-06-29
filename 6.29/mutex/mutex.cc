#include <iostream>
#include <unistd.h>
#include <pthread.h>
#define NUM 4 
using namespace std;

int ticket_num = 1000;

//用宏给锁进行初始化
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
//上锁
void* pthread_func(void* args)
{
    char* name = static_cast<char*>(args);
    while(1)
    {
        pthread_mutex_lock(&mutex);
        if(ticket_num > 0)
        {
            cout << "我是" << name << ", 我抢到票了, 剩余" << ticket_num << endl;
            ticket_num--;
            pthread_mutex_unlock(&mutex);
            //usleep(2000);
            //模拟抢票后的行为
            //如果不休眠, 某个线程在进入临界区后, 时间片到了, 切换了其他的线程被调度
            //这个新线程把变量值改了, 当原线程被切回时, 仍然保留的是之前的值
            //就导致新线程的更改失效, 变量值也有可能被减到负数 
        }
        else
        {
            pthread_mutex_unlock(&mutex);
            break;
        }
        //usleep(100);
    }
}

int main()
{
    pthread_t tid[NUM];
    for(int i = 0; i < NUM; ++i)
    {
        char* name = new char[64];
        snprintf(name, 64, "%d号进程", i + 1);
        pthread_create(tid + i, nullptr, pthread_func, name);
    }

    for(int i = 0; i < NUM; ++i)
    {
        pthread_join(tid[i], nullptr);
    }
    return 0;
}

//无锁版
// void* pthread_func(void* args)
// {
//     char* name = static_cast<char*>(args);
//     while(1)
//     {
//         if(ticket_num > 0)
//         {
//             cout << "我是" << name << ", 我抢到票了, 剩余" << ticket_num << endl;
//             ticket_num--;
//             usleep(2000);
//             //模拟抢票后的行为
//             //如果不休眠, 某个线程在进入临界区后, 时间片到了, 切换了其他的线程被调度
//             //这个新线程把变量值改了, 当原线程被切回时, 仍然保留的是之前的值
//             //就导致新线程的更改失效, 变量值也有可能被减到负数 
//         }
//         else
//         {
//             break;
//         }
//         usleep(100);
//     }
// }