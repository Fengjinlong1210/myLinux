#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
using namespace std;

int cnt = 0;

void showcnt(int signum)
{
   cout << "cnt = " << cnt << endl;
   exit(2);
}

int main()
{
    signal(SIGALRM, showcnt);
    alarm(1);
    //5秒后，发送SIGALRM信号
    while(1)
    {
        //cout << "cnt = " << cnt++ << endl;
        cnt++;
    }
    return 0;
}