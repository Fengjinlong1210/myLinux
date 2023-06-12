#include <iostream>
#include <unistd.h>
#include <signal.h>

using namespace std;
//SIGFPE  Floating-point exception 浮点数异常测试

void handler(int signo)
{
    cout << "进程收到了" << signo << "号信号退出了！" << endl;
    exit(signo);
}

// int main()
// {
//     //除0问题
//     signal(SIGFPE, handler);

//     sleep(2);
//     int a = 10;
//     a /= 0;
//     cout << "除0问题" << endl;

//     return 0;
// }

int main()
{
    //野指针问题
    signal(SIGSEGV, handler);

    int* ptr = (int*)0;
    *ptr = 0;
    //会引发段错误
    return 0;
}