#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <cstring>
#include <sys/types.h>

using namespace std;

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

void handler(int signo)
{
    cout << "对信号" << signo << "执行捕捉动作" << endl;
	int cnt = 100;
	while(cnt--)
	{
		sigset_t pend;
		sigemptyset(&pend);
		sigpending(&pend);
		PrintSigSet(pend);
		sleep(1);
	}
}

int main()
{
    struct sigaction act, oldact;
    memset(&act, 0, sizeof(act));
    memset(&oldact, 0, sizeof(oldact));

    //设置act的成员
    act.sa_flags = 0;
    act.sa_handler = handler;
    sigaddset(&act.sa_mask, 2);
    sigaddset(&act.sa_mask, 3);
    sigaddset(&act.sa_mask, 4);
    sigaddset(&act.sa_mask, 5);
    sigaddset(&act.sa_mask, 6);


    sigaction(2, &act, &oldact);


    while(1)
    {
		cout << "pid: " << getpid() << endl;
        sleep(1);
    }
    return 0;
}