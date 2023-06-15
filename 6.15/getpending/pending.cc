#include <iostream>
#include <unistd.h>
#include <signal.h>
using namespace std;

void handler(int signo)
{
    cout << "对信号" << signo << "执行捕捉动作" << endl;
}

void PrintSigSet(sigset_t set)
{
    cout << "当前进程的pending位图" << endl;
    int i = 0;
    for(i = 1; i <= 31; ++i)
    {
        if(sigismember(&set, i))
        {
            cout << "1";
        }
        else
        {
            cout << "0";
        }
    }
    cout << endl;
}

int main()
{
    //1.先更改2号指令的捕捉动作
    signal(SIGINT, handler);

    //2.屏蔽二号信号
    // 2.1 初始化set
    sigset_t set, oldset;
    sigemptyset(&set);
    sigemptyset(&oldset);
    
    //2.2 给set添加屏蔽信号
    sigaddset(&set, SIGINT);

    //2.3 将set设置进block信号集
    sigprocmask(SIG_SETMASK, &set, &oldset);

    //3.获取pending信号集, 检查2号信号状态
    sigset_t sigpend;
    sigemptyset(&sigpend);
    int cnt = 0;
    while(1)
    {
        //获取pending信号集
        int ret = sigpending(&sigpend);
        
        //打印信号集
        PrintSigSet(sigpend);

        sleep(1);

        //十秒后解除对2号信号block的限制
        if(cnt++ == 10)
        {
            sigprocmask(SIG_SETMASK, &oldset, &set);
        }
    }

    return 0;
}