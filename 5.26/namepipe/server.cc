#include <iostream>
#include <cerrno>
#include <cstring>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstdio>
#include <unistd.h>
#include "comm.hpp"
using namespace std;

int main()
{
    //服务器端创建命名管道
    int piperet = mkfifo(fifoname.c_str(), mode);
    if(piperet != 0)
    {  
        cout << "error:" << strerror(errno) << endl;
    }
    cout << "管道创建成功" << endl;

    //打开管道文件 
    int rfd = open(fifoname.c_str(), O_RDONLY);
    if(rfd == -1)
    {
        cout << "error:" << strerror(errno) << endl;
    }
    cout << "管道文件打开成功" << endl;
    //进行通信
    char buffer[1024];
    while(1)
    {
        buffer[0] = 0;
        ssize_t n = read(rfd, buffer, sizeof(buffer) - 1);
        if(n > 0)
        {
            buffer[n] = 0;
            cout << "I get :" << buffer << endl;
            //printf("%s", buffer);
            //fflush(stdout);
        }
        else if(n == 0)
        {
            cout << "client quit, me too!" << endl;
            break;
        }
        else
        {
            cout << "error:" << strerror(errno) << endl;
        }
    }
    close(rfd);
    return 0;
}
