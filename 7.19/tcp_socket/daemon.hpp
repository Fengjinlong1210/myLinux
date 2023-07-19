#pragma once
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <cerrno>
#include "log.hpp"
#include "error.hpp"

void Daemon()
{
    //1.忽略部分信号
    signal(SIGPIPE, SIG_IGN);
    signal(SIGCHLD, SIG_IGN);

    //2.自己不成为组长, 让子进程执行后面的任务
    if(fork() > 0)
    {
        exit(0);
    }

    //3.新建会话, 让新创建的子进程成为会话的首进程
    pid_t id = setsid();
    if((int)id == -1)
    {
        logMessage(Fatal, "deamon error, code: %d, string: %s", errno, strerror(errno));
        exit(SETSID_ERROR);
    }

    //4.可以更改守护进程的工作路径
    //chdir("/");
    // 5. 处理后续的对于0,1,2的问题, 改变fd
    int fd = open("/dev/null", O_RDWR);
    if (fd < 0)
    {
        logMessage(Fatal, "open error, code: %d, string: %s", errno, strerror(errno));
        exit(OPEN_ERR);
    }
    dup2(fd, 0);
    dup2(fd, 1);
    dup2(fd, 2);
    close(fd);
}