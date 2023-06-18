#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
using namespace std;

//通过该接口,可以实现不用父进程执行进程回收,收到信号后,该函数会回收退出的子进程
void handler(int signo)
{
    cout << "进程:" << getpid() << "捕捉到一个信号: " << signo << endl;
    sleep(5);

    while(1)
    {
        int ret = waitpid(-1, nullptr, 0);
        //第一个参数置为-1代表回收任意的子进程

        if(ret > 0)
        {
            cout << "等待成功, pid:" << ret << endl;
        }
        else
        {
            break;
        }
    }
    cout << "handler done" << endl;
}

int main()
{
    signal(SIGCHLD, handler);
    int i = 0;
    for(i = 0; i < 10; ++i)
    {
        int pid = fork();
        if(pid == 0)
        {
            int cnt = 5;
            while(cnt--)
            {
                cout << "我是子进程, pid :" << getpid() << ", ppid: "<< getppid() << endl; 
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