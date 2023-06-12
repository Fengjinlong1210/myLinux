#include <iostream>
#include <unistd.h>
#include <signal.h>

using namespace std;

void showBlock(sigset_t *oset)
{
    int signo = 1;
    for(; signo <=31; signo++)
    {
        if(sigismember(oset, signo)) cout << "1";
        else cout << "0";
    }
    cout << endl;
}

int main()
{
    sigset_t set, oset;
    sigemptyset(&set);
    sigemptyset(&oset);
    sigaddset(&set, 2); //SIGINT

    // 设置进入进程，谁调用，设置谁
    sigprocmask(SIG_SETMASK, &set, &oset); // 1. 2号信号没有反应 2. 我们看到老的信号屏蔽字block位图是全零、
    int cnt = 0;
    while(true)
    {
        showBlock(&set);
        sleep(1);
        cnt++;

        if(cnt == 10)
        {
            cout << "recover block" << endl;
            sigprocmask(SIG_SETMASK, &oset, &set);
            showBlock(&set); //下节课
        }
    }

    return 0;
}