#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <string>
using namespace std;

//将tid转化成16进制地址
string DecToHex(pthread_t tid)
{
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "0x%x", tid);
    return buffer;
}

void* pthread_func(void* args)
{
    int* id = (int*)(args);
    while(1)
    {
        cout << "我是" << *id<<"号线程, 我的id是: " << DecToHex(pthread_self()) << endl;
        sleep(1);
    }
}

int main()
{
    pthread_t tid[4];
    for(int i = 0; i < 4; ++i)
    {
        int* id = new int;
        *id = i + 1;
        pthread_create(tid + i, nullptr, pthread_func, id);
        pthread_detach(tid[i]);
    }


    while(1)
    {
        cout << "我是主线程" << endl;
        sleep(1);
    }
    return 0;
}