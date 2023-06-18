#include <iostream>
#include <unistd.h>
#include <signal.h>

using namespace std;

int main()
{
    signal(SIGCHLD, SIG_IGN);
    //在linux下, 将SIGCHLD的处理动作改为SIG_IGN
    //fork出来的子进程会被自动清理, 不会产生僵尸进程, 也不会通知父进程
    int i = 0;
    for(; i < 10; ++i)
    {
        int pid = fork();
        if(pid == 0)
        {
            int cnt = 5;
            while(cnt--)
            {
                cout << "我是子进程, pid :" << getpid() << endl;
                sleep(1);
            }
            exit(1);
        }
    }

    while(1)
    {
        sleep(1);
    }
    return 0;
}