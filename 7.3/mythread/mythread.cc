#include <iostream>
#include <unistd.h>
#include <pthread.h>
using namespace std;
#define NUM 5
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void* thread_func(void* args)
{
    const char* name = static_cast<const char*>(args);
    while(1)
    {
        pthread_mutex_lock(&mutex);
        pthread_cond_wait(&cond, &mutex);
        cout << name << "运行" << endl;
        pthread_mutex_unlock(&mutex);
    }
}

int main()
{
    pthread_t tids[NUM];
    for(int i = 0; i < NUM; ++i)
    {
        char* name = new char[32];
        snprintf(name, 32, "%d号线程", i + 1);
        pthread_create(tids + i, nullptr, thread_func, name);
        //delete[] name;
    }
    
    sleep(3);
    while(1)
    {
        cout << "main线程唤醒其他线程" << endl;
        //pthread_cond_signal(&cond);//按顺序调用
        pthread_cond_broadcast(&cond);
        sleep(1);
    }

    for(int i = 0; i < NUM; ++i)
    {
        pthread_join(tids[i], nullptr);
    }
    return 0;
}

// void* thread_func(void* args)
// {
//     cout << "我是新线程" << endl;
//     pthread_mutex_lock(&mutex); 
//     cout << "申请mutex成功" << endl;
//     //阻塞等待主线程释放
//     pthread_mutex_lock(&mutex); 
//     cout << "再次申请成功" << endl;
// }

// int main()
// {
//     pthread_t tid;
//     pthread_create(&tid, nullptr, thread_func, nullptr);

//     sleep(3);
//     pthread_mutex_unlock(&mutex);
//     cout << "main释放mutex" << endl;
//     //释放后, 新线程可以再次申请
//     //可以通过其他线程控制锁的释放
//     return 0;
// }