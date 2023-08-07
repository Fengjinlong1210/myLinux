#pragma once
#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <sys/types.h>
#include "socket.hpp"

typedef pollfd type_t; //

static const uint16_t default_port = 8888;
static const int default_fd = -1;
static const int N = 4096;
static const int default_event = 0;
class PollServer
{
public:
    PollServer(uint16_t port = default_port)
        : _port(port)
    {
    }
    ~PollServer() {}

    void init_server()
    {
        // 初始化套接字
        _listensock.Socket();
        _listensock.Bind(_port);
        _listensock.Listen();
        _fd_array = new type_t[N];
        for (int i = 0; i < N; ++i)
        {
            _fd_array[i].fd = default_fd;
            _fd_array[i].events = default_event;
            _fd_array[i].revents = default_event;
        }
    }

    void start_server()
    {
        // 要将listensocket设置进数组内
        // listensocket也会被当成读事件就绪
        // 在网络中, 新的连接到来也会被当成读事件就绪
        _fd_array[0].fd = _listensock.get_socket();
        _fd_array[0].events = POLLIN;
        while (1)
        {
            int timeout = 0; //-1表示阻塞式等待
            // 0表示非阻塞, >0表示等待的微秒数
            int n = poll(_fd_array, N, timeout);
            switch (n)
            {
            case 0:
                logMessage(Debug, "timeout, 我正在执行其他任务, %d, %s", errno, strerror(errno));
                break;
            case -1:
                logMessage(Warning, "%d, %s", errno, strerror(errno));
                break;
            default:
                // 等待成功了
                logMessage(Info, "有一个事件就绪");
                // 调用处理函数
                HandlerEvent();
                break;
            }
            sleep(1);
        }
    }

    void HandlerEvent()
    {
        // 遍历fd_array, 找到就绪的fd
        for (int i = 0; i < N; ++i)
        {
            int fd = _fd_array[i].fd;
            short revent = _fd_array[i].revents;
            if (fd == default_fd)
                continue;
            //cout << "当前的fd: " << fd << endl;
            if (fd == _listensock.get_socket() && (revent & POLLIN))    //revent是返回的就绪事件
            {
                // 当前是listen并且读事件已经就绪
                // 也就是新连接到来了
                //logMessage(Debug, "before Accepter");
                Accepter();
                //logMessage(Debug, "after Accepter");
            }
            else if (revent & POLLIN)
            {
                // 当前不是listen, 读事件就绪, 需要进行IO
                //logMessage(Debug, "before IOService");
                IOService(i);
                //logMessage(Debug, "after IOService");
            }
        }
    }

    void Accepter()
    {
        // cout << "Accepter" << endl;
        // 接收客户端发来的连接请求
        // 将客户端fd也存放进fd数组
        string client_ip;
        uint16_t client_port;
        //logMessage(Debug, "before socket accept");
        int client_sock = _listensock.Accept(&client_ip, &client_port);
        //logMessage(Debug, "before socket accept");
        if (client_sock < 0)
            return;
        // 找到一个空闲的fd下标
        int pos = 1;
        for (; pos < N; ++pos)
        {
            if (_fd_array[pos].fd == default_fd)
                break;
        }
        //logMessage(Debug, "pos = %d", pos);
        if (pos >= N)
        {
            logMessage(Warning, "fd_array is full");
            close(client_sock);
        }
        else
        {
            // 当前的pos就是空闲的下标
            _fd_array[pos].fd = client_sock;
            _fd_array[pos].events = POLLIN;
            _fd_array[pos].revents = default_event;
        }
    }

    void IOService(int i)
    {
        // cout << "IOServer" << endl;
        // 拿取当前的fd
        int fd = _fd_array[i].fd;
        char buffer[1024];
        ssize_t n = recv(fd, buffer, sizeof(buffer) - 1, 0);
        if (n > 0)
        {
            buffer[n - 1] = 0;
            buffer[n - 2] = 0;
            cout << "client: " << buffer << endl;
            string resp = buffer;
            resp += " server echo: ";
            send(fd, resp.c_str(), resp.size(), 0);
            logMessage(Debug, "i am here");
        }
        else
        {
            // 读取异常
            if (n == 0)
            {
                // 对端关闭连接了
                logMessage(Info, "client quit");
            }
            else
            {
                // n < 0
                logMessage(Warning, "recv error, errno: %d, errstring: %s", errno, strerror(errno));
            }
            close(fd);
            _fd_array[i].fd = default_fd;
            _fd_array[i].events = default_event;
            _fd_array[i].revents = default_event;
        }
    }

    // void print_fd()
    // {
    //     for (int i = 0; i < N; ++i)
    //     {
    //         if (_fd_array[i] == default_fd)
    //             continue;
    //         cout << i << ":" << _fd_array[i] << " ";
    //     }
    //     cout << endl;
    // }

private:
    sock _listensock;
    uint16_t _port;
    type_t *_fd_array;
    // 设置一个数组用来保存所有的文件描述符
};