#pragma once
#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>
#include <signal.h>
#include <cerrno>
#include <functional>
#include <pthread.h>
#include "error.hpp"
namespace Fjl
{
    using func_t = std::function<std::string(std::string)>;
    const int backlog = 32;
    class tcp_server;
    struct thread_tcp
    {
        thread_tcp(int sock, const std::string client_ip, const uint16_t client_port, tcp_server *ts)
            : _sock(sock), _client_ip(client_ip), _client_port(client_port), _ts(ts)
        {
        }

        int _sock;
        std::string _client_ip;
        uint16_t _client_port;
        tcp_server *_ts;
    };

    class tcp_server
    {
    public:
        tcp_server(func_t func, uint16_t port = 8080)
            : _port(port), _func(func)
        {
        }

        void server_init()
        {
            // 1.创建套接字
            _listen_sock = socket(AF_INET, SOCK_STREAM, 0);
            if (_listen_sock < 0)
            {
                std::cout << "create socket error: " << strerror(errno) << std::endl;
                exit(SOCKET_ERROR);
            }
            // std::cout << "socket = " << _listen_sock << std::endl;
            //  2.绑定
            sockaddr_in local;
            local.sin_addr.s_addr = htonl(INADDR_ANY);
            local.sin_family = AF_INET;
            local.sin_port = htons(_port);
            if (bind(_listen_sock, (sockaddr *)&local, sizeof(local)) < 0)
            {
                std::cout << "socket bind error: " << strerror(errno) << std::endl;
                exit(BIND_ERROR);
            }

            // 3.监听
            if (listen(_listen_sock, backlog))
            {
                std::cout << "socket listen error: " << strerror(errno) << std::endl;
                exit(LISTEN_ERROR);
            }
            // std::cout << "all success" << std::endl;
        }

        void server_start()
        {
            // signal(SIGCHLD, SIG_IGN);
            // 将SIGCHLD设置为忽略后, 子进程退出后回自动被回收
            while (1)
            {
                // 获取客户端连接 accept
                sockaddr_in client;
                socklen_t length;
                int sock = accept(_listen_sock, (sockaddr *)&client, &length);
                if (sock < 0)
                {
                    std::cout << "accept client error" << strerror(errno) << std::endl;
                    continue;
                }
                std::string client_ip = inet_ntoa(client.sin_addr);
                uint16_t client_port = ntohs(client.sin_port);

                std::cout << "获取新连接成功: " << sock << "from" << _listen_sock << ","
                          << client_ip << ": " << client_port << std::endl;

                // v1: 只有一个执行流, 只能连接一个客户端
                // service(sock, client_ip, client_port);

                // v2: 多进程版本, 用多个进程实现多个客户端
                //  pid_t id = fork();
                //  if(id < 0)
                //  {
                //      //子进程创建失败, 重新循环
                //      close(sock);
                //      continue;
                //  }
                //  else if(id == 0)
                //  {
                //      //子进程, 拷贝了父进程的文件描述符表
                //      close(_listen_sock);    //关闭父进程的文件描述符
                //      if(fork() > 0) exit(0);     //这一行的意思是, 子进程再创建一个孙子进程
                //      //让孙子进程执行下面的代码, 子进程直接退出, 这时孙子进程就是一个孤儿进程
                //      //孤儿进程会被OS接管, 会被自动回收
                //      service(sock, client_ip, client_port);
                //      //执行完毕退出
                //      exit(0);
                //  }
                //  //父进程把任务交给子进程后, 关闭文件描述符, 为下一个客户端做准备
                //  //防止文件描述符过多
                //  close(sock);

                // v3: 多线程版本, 创建线程来执行service函数
                thread_tcp *p_thread = new thread_tcp(sock, client_ip, client_port, this);
                pthread_t tid;
                pthread_create(&tid, nullptr, thread_func, p_thread);
            }
        }

        static void *thread_func(void *args)
        {
            pthread_detach(pthread_self());
            //解除绑定, 线程无需join
            thread_tcp *p_thread = static_cast<thread_tcp *>(args);
            p_thread->_ts->service(p_thread->_sock, p_thread->_client_ip, p_thread->_client_port);
            //执行后delete, 关闭文件描述符
            delete p_thread;
            return nullptr;
        }

        void service(int sock, const std::string client_ip, const uint16_t client_port)
        {
            std::string client = client_ip + ":" + to_string(client_port);
            char buffer[2048];
            while (1)
            {
                int n = read(sock, buffer, sizeof(buffer));
                if (n > 0)
                {
                    buffer[n] = 0;
                    std::string res = _func(buffer);
                    std::cout << client << ">> " << res << std::endl;

                    write(sock, res.c_str(), res.size());
                }
                else if (n == 0)
                {
                    // 客户端关闭了连接
                    close(sock); //  关闭文件描述符
                    std::cout << client << " quit" << std::endl;
                    break;
                }
                else
                {
                    close(sock);
                    std::cerr << "read error" << strerror(errno) << std::endl;
                    break;
                }
            }
        }

        ~tcp_server()
        {
        }

    private:
        int _listen_sock;
        uint16_t _port;
        func_t _func;
    };
}