#include <iostream>
#include <cerrno>
#include <cstring>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstdio>
#include <unistd.h>
#include "comm.hpp"
#include <string>
using namespace std;

int main()
{
    //打开管道文件
    int wfd = open(fifoname.c_str(), O_WRONLY);
    if(wfd == -1)
    {
        cout << "error:" << strerror(errno) << endl;
        return 1;
    }

    //通信
    char buffer[1024];
    string str;
    cout << "向服务器发送信息：";
    while(getline(cin, str))
    {
        //获取输入
        cout << "向服务器发送信息：";
        
        buffer[strlen(buffer) - 1] = 0;
        //cout << buffer;
        if(strcasecmp(buffer, "quit") == 0)
        {
            break;
        }
        //向文件写入
        //int wrret = write(wfd, buffer, sizeof(buffer));
        int wrret = write(wfd, str.c_str(), sizeof(str));
        if(wrret < 0)
        {
            cout << "error:" << strerror(errno) << endl;
        }
    }
    //关闭管道文件
    close(wfd);
    unlink(fifoname.c_str());
    return 0;
}