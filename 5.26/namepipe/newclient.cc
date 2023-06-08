#include <iostream>
#include <cstdio>
#include <cerrno>
#include <cstring>
#include <cassert>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
// #include <ncurses.h>
#include "comm.hpp"

int main()
{
    //1. 不需创建管道文件，我只需要打开对应的文件即可！
    int wfd = open(fifoname.c_str(), O_WRONLY);
    if(wfd < 0)
    {
        std::cerr << errno << ":" << strerror(errno) << std::endl;
        return 1;
    }

    // 可以进行常规通信了
    char buffer[NUM];
    while(true)
    {
        std::cout << "请输入你的消息# ";
        char *msg = fgets(buffer, sizeof(buffer), stdin);
        //assert(msg);
        // (void)msg;
        // int c = getch();
        // std::cout << c << std::endl;
        // if(c == -1) continue;

        //system("stty raw");
        //int c = getchar();
        //system("stty -raw");

        //std::cout << c << std::endl;
        //sleep(1);

        buffer[strlen(buffer) - 1] = 0;
        // abcde\n\0
        // 012345
        if(strcasecmp(buffer, "quit") == 0) break;

        //ssize_t n = write(wfd, (char*)&c, sizeof(char));
        ssize_t n = write(wfd, buffer, sizeof(buffer));
        assert(n >= 0);
        (void)n;
    }

    close(wfd);

    return 0;
}