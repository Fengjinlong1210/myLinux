#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <string>
#include <ctime>
#define NUM 5
using namespace std;

class thread
{
public:
    thread(string name, time_t time, pthread_t id)
        : _name(name), _time(time), _id(id)
    {}

    void thread_func()
    {
        printf("我是%d号进程, 正在执行某个任务!\n", _id);
    }

private:
    string _name;
    time_t _time;
    pthread_t _id;

};

void *pthread_func(void *args)
{
    int id = *((int*)args);
    string name = "线程名";
    thread t1(name, time(nullptr), id);
    //printf("args = %p\n", args);
    t1.thread_func();
}

// 创建多个线程, 让多个线程执行自己的任务
int main()
{
    pthread_t tids[5];
    for (int i = 0; i < NUM; ++i)
    {
        int* id = new int;
        *id = i + 1;
        pthread_create(tids + i, nullptr, pthread_func, (void*)(id));
        //第四个参数可以传任意类型的指针, 包括类的指针
        sleep(1);
    }

    return 0;
}

// void *pthread_func(void *args)
// {
//     char *arg = (char *)args;
//     while (1)
//     {
//         cout << "我是" << arg << ", 我的id是:" << pthread_self() << endl;
//         sleep(1);
//     }
//     return nullptr;
// }

// int main()
// {
//     // 创建十个线程, 要求每个线程都打印出自己的编号和id
//     pthread_t tids[10];
//     for (int i = 0; i < NUM; ++i)
//     {
//         //char buffer[128];
//         //如果使用主线程的数组, 每次都会打印相同的线程号
//         //因为其他的线程都能访问到公共的数组, 为了让线程看到不同的号码,
//         // 要为每个线程开辟不同的空间
//         char* buffer = new char[64];
//         snprintf(buffer, 64, "%d号线程", i + 1);
//         pthread_create(tids + i, nullptr, pthread_func, buffer);
//         sleep(1);
//     }
//     while (1)
//     {
//         cout << "我是主线程" << endl;
//         sleep(1);
//     }

//     // 回收
//     for (int i = 0; i < NUM; ++i)
//     {
//         pthread_join(tids[i], nullptr);
//     }
// }

// void* thread_func(void* args)
// {
//     //线程执行函数
//     //通常线程函数要先获取参数
//     char* arg = (char*) args;
//     while(1)
//     {
//         cout << arg << ", 我的ID: " << pthread_self() << endl;
//         sleep(1);
//     }
//     pthread_exit((void*)0);
// }

// int main()
// {
//     //创建线程
//     pthread_t tid;
//     char buffer[1024] = "我是一个线程";
//     pthread_create(&tid, nullptr, thread_func, buffer);
//     while(1)
//     {
//         cout << "我是主线程" << endl;
//         sleep(1);
//     }
//     //回收线程
//     pthread_join(tid, nullptr);
//     return 0;
// }