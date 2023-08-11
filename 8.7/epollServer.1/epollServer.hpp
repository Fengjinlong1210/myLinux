#pragma once
#include <iostream>
#include <string>
#include <functional>
#include <unistd.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <sys/types.h>
#include "socket.hpp"
#include "epoll.hpp"
#include "epoll.hpp"

using func_t = std::function<std::string(std::string)>;
class epollServer
{
    static const int gnum = 64;
    const static uint16_t default_port = 8888;
public:
    epollServer(func_t func, uint16_t port = default_port)
        : _func(func), _port(port)
    {
    }
    ~epollServer() {}

    void init_server()
    {
        // 初始化套接字
        _listensock.Socket();
        _listensock.Bind(_port);
        _listensock.Listen();
        _epoll.Create();
        logMessage(Debug, "initialize EpollServer success");
    }

    void start_server()
    {
        // 要将listensocket设置进数组内
        // listensocket也会被当成读事件就绪
        // 在网络中, 新的连接到来也会被当成读事件就绪
        bool ret = _epoll.AddEvent(_listensock.get_socket(), EPOLLIN);
        if (ret)
        {
            logMessage(Debug, "Add listen socket success");
        }
        else
        {
            logMessage(Fatal, "Add listen socket error, errno: %d, errstring: %s", errno, strerror(errno));
            exit(EPOLL_CTL_ADD_ERR);
        }
        int timeout = -1; // 阻塞式
        while (1)
        {
            int n = _epoll.Wait(_recv, gnum, timeout);
            switch (n)
            {
            case 0:
                logMessage(Debug, "timeout, 我正在执行其他任务, %d, %s", errno, strerror(errno));
                break;
            case -1:
                logMessage(Warning, "epoll_wait failed, %d, %s", errno, strerror(errno));
                break;
            default:
                // 等待成功了
                logMessage(Info, "有%d个事件就绪", n);
                // 调用处理函数
                HandlerEvents(n);
                break;
            }
            sleep(1);
        }
    }

    void HandlerEvents(int n)
    {
        // 有n个事件就绪
        for (int i = 0; i < n; ++i)
        {
            int fd = _recv[i].data.fd;
            uint32_t event = _recv[i].events;
            logMessage(Info, "我正在处理%d号文件描述符中的%s事件", fd, (event & EPOLLIN) ? "EPOLLIN" : "OTHER");
            if (event & EPOLLIN)
            {
                if (fd == _listensock.get_socket())
                {
                    Accepter();
                }
                else
                {
                    IOService(fd);
                }
            }
        }
    }

    void Accepter()
    {
        // cout << "Accepter" << endl;
        // 接收客户端发来的连接请求
        string client_ip;
        uint16_t client_port;
        // logMessage(Debug, "before socket accept");
        int client_sock = _listensock.Accept(&client_ip, &client_port);
        // logMessage(Debug, "before socket accept");
        if (client_sock < 0)
            return;
        logMessage(Info, "%d-%s已经连上了服务器", client_sock, client_ip.c_str());
        bool ret = _epoll.AddEvent(client_sock, EPOLLIN);
        if (ret)
        {
            logMessage(Debug, "Add event fd success");
        }
        else
        {
            logMessage(Fatal, "Add event fd error, errno: %d, errstring: %s", errno, strerror(errno));
            exit(EPOLL_CTL_ADD_ERR);
        }
    }

    void IOService(int fd)
    {
        // cout << "IOServer" << endl;
        // 拿取当前的fd
        
        char request[1024];
        ssize_t n = recv(fd, request, sizeof(request) - 1, 0);
        if (n > 0)
        {
            request[n - 1] = 0;
            request[n - 2] = 0;
            string resp = _func(request);
            send(fd, resp.c_str(), resp.size(), 0);
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
            //先从epoll中移除
            _epoll.DelEvent(fd);
            close(fd);
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
    struct epoll_event _recv[gnum];
    // 设置一个数组用来保存所有的已经就绪的事件结构体
    Epoll _epoll;
    func_t _func;
};
