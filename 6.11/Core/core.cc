#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

using namespace std;


int main()
{
    int pid = fork();
    if(pid == 0)
    {
        //子进程
        cout << "我会触发段错误" << endl;
        cout << "我会触发段错误" << endl;
        cout << "我会触发段错误" << endl;

        int* ptr = (int*)0;
        *ptr = 100;

        cout << "我会触发段错误" << endl;
        cout << "我会触发段错误" << endl;
        cout << "我会触发段错误" << endl;
    }
    int status = 0;
    waitpid(pid, &status, 0);

    sleep(1);
    cout << "exit code : " << ((status >> 8) & 0xFF) << endl;
    cout << "exit signal : " << (status & 0x7F) << endl;
    cout << "core dump flag : " << ((status >> 7) & 0x1) << endl; 
    return 0;
}