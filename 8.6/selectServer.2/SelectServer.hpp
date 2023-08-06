#pragma once
#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include "socket.hpp"

// v2版本的selectserver, 增加了写事件的监测
#define READ_EVENT 0x1
#define WRITE_EVENT (0x1 << 1)
#define EXCEPT_EVENT (0x1 << 2)

// 定义一个结构体用来存储客户端的信息和事件(读、写、异常)
typedef struct
{
    int fd;
    uint8_t event;
    string client_ip;
    uint16_t client_port;
} type_t; //

const uint16_t default_port = 8888;
const int default_fd = -1;
const int default_event = 0;
class SelectServer
{
    static const int fd_size = sizeof(fd_set) * 8;

public:
    SelectServer(uint16_t port = default_port)
        : _port(port)
    {
    }
    ~SelectServer() {}

    void init_server()
    {
        // 初始化套接字
        _listensock.Socket();
        _listensock.Bind(_port);
        _listensock.Listen();
        for (int i = 0; i < fd_size; ++i)
        {
            // 初始化fd数组
            _fd_array[i].fd = default_fd;
            _fd_array[i].event = default_event;
            _fd_array[i].client_port = 0;
        }
    }

    void start_server()
    {
        // 要将listensocket设置进数组内
        // listensocket也会被当成读事件就绪
        // 在网络中, 新的连接到来也会被当成读事件就绪
        _fd_array[0].fd = _listensock.get_socket();
        _fd_array[0].event = READ_EVENT;
        while (1)
        {
            // select中的位图是一个输入输出结构, 注定了每次都会对历史上的fd进行重制
            // 注定了要保存所有的fd
            // 服务器在运行时, fd的数值大小也会动态变化, 所以要记录当前最大的fd
            // 因此要创建一个数组来维护历史上的fd
            fd_set rdfds;
            fd_set wtfds;

            FD_ZERO(&rdfds); // 清空位图
            FD_ZERO(&wtfds); // 清空位图

            int maxfd = _fd_array[0].fd;
            // 遍历文件描述符数组, 将存在的fd放入可读fd位图
            for (int i = 0; i < fd_size; ++i)
            {
                if (_fd_array[i].fd == default_fd)
                    continue; // 该描述符不存在
                // 如果存在, 就放入读取位图中
                // cout << "_fd_array = " << _fd_array[i].fd << endl;

                // 是哪种事件就放入哪种位图中
                if (_fd_array[i].event & READ_EVENT)
                    FD_SET(_fd_array[i].fd, &rdfds);
                if (_fd_array[i].event & WRITE_EVENT)
                    FD_SET(_fd_array[i].fd, &wtfds);

                if (_fd_array[i].fd > maxfd)
                    maxfd = _fd_array[i].fd;
            }
            timeval tm = {2, 0};

            int n = select(maxfd + 1, &rdfds, &wtfds, nullptr, &tm);
            // int n = select(maxfd + 1, &rdfds, nullptr, nullptr, nullptr);

            // cout << "select success" << endl;
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
                HandlerEvent(rdfds, wtfds);
                // 打印当前的fd数组
                print_fd();
                break;
            }
        }
    }

    void HandlerEvent(fd_set &rdfds, fd_set &wtfds)
    {
        // 遍历一遍fd位图, 对读事件就绪的fd进行处理
        for (int i = 0; i < fd_size; ++i)
        {
            if (_fd_array[i].fd == default_fd)
                continue;

            if (_fd_array[i].event & READ_EVENT && FD_ISSET(_fd_array[i].fd, &rdfds))
            {
                if (_fd_array[i].fd == _listensock.get_socket())
                {
                    // 当前是listen并且读事件已经就绪
                    // 也就是新连接到来了
                    Accepter();
                }
                else if (FD_ISSET(_fd_array[i].fd, &rdfds))
                {
                    // 当前不是listen, 读事件就绪, 需要进行IO
                    IOService(i);
                }
            }
            else if (_fd_array[i].event & WRITE_EVENT && FD_ISSET(_fd_array[i].fd, &wtfds))
            {
                // 写事件准备就绪
            }
            else
            {
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
        int client_sock = _listensock.Accept(&client_ip, &client_port);
        if (client_sock < 0)
            return;
        // 找到一个空闲的fd下标
        int pos = 1;
        for (; pos < fd_size; ++pos)
        {
            if (_fd_array[pos].fd == default_fd)
                break;
        }
        if (pos >= fd_size)
        {
            logMessage(Warning, "fd_array is full");
            close(client_sock);
        }
        else
        {
            // 当前的pos就是空闲的下标
            _fd_array[pos].fd = client_sock;
            _fd_array[pos].event = READ_EVENT;
            _fd_array[pos].client_ip = client_ip;
            _fd_array[pos].client_port = client_port;
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
            cout << "client: " << buffer << endl;
            string resp = buffer;
            resp += " server echo: ";
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
            close(fd);
            _fd_array[i].fd = default_fd;
            _fd_array[i].event = default_event;
            _fd_array[i].client_ip.resize(0);
            _fd_array[i].client_port = 0;
        }
    }

    void print_fd()
    {
        for (int i = 0; i < fd_size; ++i)
        {
            if (_fd_array[i].fd == default_fd)
                continue;
            cout << i << ":" << _fd_array[i].fd << " ";
        }
        cout << endl;
    }

private:
    sock _listensock;
    uint16_t _port;
    type_t _fd_array[fd_size];
    // 设置一个数组用来保存所有的文件描述符
};