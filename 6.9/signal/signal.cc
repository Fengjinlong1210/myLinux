#include <iostream>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>

using namespace std;

//typedef void(*handler)(int);

void handler(int signum)
{
    cout << signum << "已经被替换成handler:" << endl;
}

int main()
{
    //改变所有信号的处理动作
    int i = 0;
    for(i = 1; i < 32; ++i)
    {
        signal(i, handler);
    }

    //改变2号信号的的处理动作
    // signal(SIGINT, handler);
    while(1)
    {
        cout << "我是一个进程，我在运行  pid: " << getpid() << endl;
        sleep(1);
    }
    return 0;
}