#pragma once
#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include "socket.hpp"

typedef int type_t; //

const uint16_t default_port = 8888;
const int default_fd = -1;
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
            _fd_array[i] = default_fd;
        }
    }

    void start_server()
    {
        // 要将listensocket设置进数组内
        // listensocket也会被当成读事件就绪
        // 在网络中, 新的连接到来也会被当成读事件就绪
        _fd_array[0] = _listensock.get_socket();
        while (1)
        {
            // select中的位图是一个输入输出结构, 注定了每次都会对历史上的fd进行重制
            // 注定了要保存所有的fd
            // 服务器在运行时, fd的数值大小也会动态变化, 所以要记录当前最大的fd
            // 因此要创建一个数组来维护历史上的fd
            fd_set rdfds;
            FD_ZERO(&rdfds); // 清空位图
            int maxfd = _fd_array[0];
            // 遍历文件描述符数组, 将存在的fd放入可读fd位图
            for (int i = 0; i < fd_size; ++i)
            {
                if (_fd_array[i] == default_fd)
                    continue; // 该描述符不存在
                // 如果存在, 就放入读取位图中
                //cout << "_fd_array = " << _fd_array[i] << endl;
                FD_SET(_fd_array[i], &rdfds);
                if (_fd_array[i] > maxfd)
                    maxfd = _fd_array[i];
            }
            timeval tm = {2, 0};
            
            int n = select(maxfd + 1, &rdfds, nullptr, nullptr, &tm);
            //int n = select(maxfd + 1, &rdfds, nullptr, nullptr, nullptr);

            //cout << "select success" << endl;
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
                HandlerEvent(rdfds);
                // 打印当前的fd数组
                print_fd();
                break;
            }
        }
    }

    void HandlerEvent(fd_set &rdfds)
    {
        // 遍历一遍fd位图, 对读事件就绪的fd进行处理
        for(int i = 0; i < fd_size; ++i)
        {
            if(_fd_array[i] == default_fd) continue;

            if(_fd_array[i] == _listensock.get_socket() && FD_ISSET(_fd_array[i], &rdfds))
            {
                //当前是listen并且读事件已经就绪
                //也就是新连接到来了
                Accepter();
            }
            else if(FD_ISSET(_fd_array[i], &rdfds))
            {
                //当前不是listen, 读事件就绪, 需要进行IO
                IOService(i);
            }
        }
    }

    void Accepter()
    {
        //cout << "Accepter" << endl;
        //接收客户端发来的连接请求
        //将客户端fd也存放进fd数组
        string client_ip;
        uint16_t client_port;
        int client_sock = _listensock.Accept(&client_ip, &client_port);
        if(client_sock < 0)
            return;
        //找到一个空闲的fd下标
        int pos = 1;
        for(; pos < fd_size; ++pos)
        {
            if(_fd_array[pos] == default_fd) break;
        }
        if(pos >= fd_size) 
        {
            logMessage(Warning, "fd_array is full");
            close(client_sock);
        }
        else
        {
            //当前的pos就是空闲的下标
            _fd_array[pos] = client_sock;
        }
    }

    void IOService(int i)
    {
        //cout << "IOServer" << endl;
        //拿取当前的fd
        int fd = _fd_array[i];
        char buffer[1024];
        ssize_t n = recv(fd, buffer, sizeof(buffer) - 1, 0);
        if(n > 0)
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
            if(n == 0)
            {
                //对端关闭连接了
                logMessage(Info, "client quit");
            }
            else
            {
                //n < 0
                logMessage(Warning, "recv error, errno: %d, errstring: %s", errno, strerror(errno));
            }
            close(fd);
            _fd_array[i] = default_fd;
        }
    }

    void print_fd()
    {
        for (int i = 0; i < fd_size; ++i)
        {
            if (_fd_array[i] == default_fd)
                continue;
            cout << i << ":" << _fd_array[i] << " ";
        }
        cout << endl;
    }

private:
    sock _listensock;
    uint16_t _port;
    type_t _fd_array[fd_size];
    // 设置一个数组用来保存所有的文件描述符
};