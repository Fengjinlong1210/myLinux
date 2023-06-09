#include <cstring>
#include <cerrno>
#include <iostream>
#include <sys/types.h>
#include <signal.h>

using namespace std;

void help()
{
    cout << "./mykill signalnum pid" << endl;
}

int main(int argc, char* argv[])//main函数的第一个参数是指向程序的路径
{
    if(argc != 3)
    {
        help();
        exit(1);
    }

    int signum = atoi(argv[1]);
    int pid = atoi(argv[2]);

    int ret = kill(pid, signum);
    if(ret == -1)
    {
        cout << "error : " << errno << strerror(errno) << endl;
        exit(2);
    }


    return 0;
}