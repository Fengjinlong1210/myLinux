#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <cstring>

using namespace std;

void* pthread_func(void* args)
{
    pthread_detach(pthread_self());
    int cnt = 5;
    while(cnt--)
    {
        cout << "我是一个线程, 我的id:" << pthread_self() << endl;
        sleep(1);
    }
}

int main()
{
    pthread_t tid;
    pthread_create(&tid, nullptr, pthread_func, nullptr);

    //sleep(1);
    //等待一秒后, 让新线程先detach, 然后再调用pthread_join
    //这时join发现该线程已经detach了, 就会返回invalid argument
    //如果不睡眠, join会在detach前调用, 这时就会阻塞式等待线程退出

    int ret = pthread_join(tid, nullptr);
    if(ret == 0)
    {
        cout << "等待线程结束成功!" << endl;
    }
    else
    {
        cout << "等待线程失败: ret = " << ret << endl;
    }

    return 0;
}