#include <iostream>
#include <unistd.h>
#include <sys/types.h>
using namespace std;

int main()
{
    while(1)
    {
         cout << "我是一个进程，我的pid是: " << getpid() << endl;
         sleep(1);
    }
    return 0;
}
