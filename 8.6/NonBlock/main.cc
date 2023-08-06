#include <iostream>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include "log.hpp"
#include "error.hpp"
using namespace std;

void SetNonBlock(int fd)
{
    int f1 = fcntl(fd, F_GETFL);
    if (f1 < 0)
    {
        logMessage(Error, "fcntl failed");
        exit(Error);
    }
    fcntl(fd, F_SETFL, f1 | O_NONBLOCK);
}

void Task()
{
    cout << "I'm doing other tasks" << endl;
}

int main()
{
    char buffer[128];
    SetNonBlock(0);
    while (1)
    {
        printf("please enter>> ");
        fflush(stdin);
        ssize_t n = read(0, buffer, sizeof(buffer) - 1);
        if (n > 0)
        {
            cout << "I get: " << buffer;
        }
        else if (n == 0)
        {
            logMessage(Info, "end of file");
            continue;
        }
        else
        {
            // 等待失败
            if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                // 没有等待到数据, 继续执行自己的任务
                Task();
                sleep(1);
            }
            else if (errno == EINTR)
            {
                // 这次读取被信号中断
                continue;
            }
            else
            {
                logMessage(Error, "errno:%d, errstring%s", errno, strerror(errno));
            }
        }
    }
    return 0;
}
