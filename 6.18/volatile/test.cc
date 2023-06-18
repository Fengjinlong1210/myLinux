#include <iostream>
#include <signal.h>
using namespace std;

//int flag = 0;

volatile int flag = 0;
//增加了volatile关键字后, 告诉编译器, 每次检测都要尝试从内存中读取数据
//不用寄存器中的数据, 保证内存数据可见


void handler(int signo)
{
    cout << "change flag from 0 to 1";
    flag = 1;
    cout << "flag = " << flag << endl;
}

int main()
{
    signal(SIGINT, handler);

    while(!flag);
    //不增加代码块,让编译器以为在main中,只会对flag进行检测
    //编译器会进行优化, 让cpu只把flag加载到寄存器上一次, 以后都会使用寄存器上的值进行判断
    //不会到内存中拿取新的flag值, 所以在handler方法中修改了内存中的flag值
    //cpu也还是会不断的用以前的值进行判断
    cout << "main quit!" << endl;

    return 0;
}