#pragma once
#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <functional>
#include <vector>
#include <unordered_map>
#include <cerrno>
#include "LockGuard.hpp"
#include "RingQueue.hpp"
#include "Thread.hpp"
#include "error.hpp"

namespace Fjl
{
    class udp_server
    {
    public:
        udp_server(uint16_t port = 8080)
            : _port_(port)
        {
            pthread_mutex_init(&_lock, nullptr);
            //给线程传入bind后的函数, 可以在类外面调用类内部的成员函数
            auto recv_func = std::bind(&udp_server::server_recive, this);
            auto broad_func = std::bind(&udp_server::server_broadcast, this);

            _recv_th = new Thread(1, recv_func);
            _broad_th = new Thread(2, broad_func);
        }

        void server_start()
        {
            // 1. 对套接字进行初始化
            _socket_ = socket(AF_INET, SOCK_DGRAM, 0);
            if (_socket_ < 0)
            {
                std::cout << "create socket error : " << strerror(errno) << std::endl;
                exit(SOCKET_ERROR);
            }
            std::cout << "create socket success, socket = " << _socket_ << std::endl;

            // 2. 给服务器指明IP地址和端口号 也就是绑定, 绑定socket和sockaddr对应对结构体
            // int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
            struct sockaddr_in local;
            local.sin_family = AF_INET;         // 协议族
            local.sin_addr.s_addr = INADDR_ANY; // IP地址
                                                // inet_addr 将字符串的ip地址转化成32位int, 同时把主机序列转化成网络序列
                                                // 云服务器或者 一款服务器, 不要指明一个确定的ip
                                                // INADDR_ANY允许udpserver启动时, 绑定本主机的任意ip
            local.sin_port = htons(_port_);     // 端口号

            //    绑定
            // 云服务器不需要绑定ip地址, 需要让服务器自己绑定ip地址
            //  本地的虚拟机或物理机是可以绑定的
            if (bind(_socket_, (struct sockaddr *)&local, sizeof(local)) < 0)
            {
                std::cout << "bind socket error : " << strerror(errno) << std::endl;
                exit(BIND_ERROR);
            }
            std::cout << "bind socket success" << std::endl;

            _recv_th->run();
            _broad_th->run();
        }

        void server_recive()
        {
            char buffer[2048];
            while (1)
            {
                // 定义一个结构体充当输出型参数
                struct sockaddr_in temp;
                socklen_t length = sizeof(temp); // 该类型本质上是一个unsigned int
                // 阻塞式接受数据
                int n = recvfrom(_socket_, buffer, sizeof(buffer) - 1, 0, (sockaddr *)&temp, &length);
                if (n > 0)
                {
                    buffer[n] = 0;
                }
                else
                {
                    continue;
                }

                // 读到以后提取客户端信息
                std::string client_ip = inet_ntoa(temp.sin_addr); // 该接口将结构体转化成字符串
                uint16_t client_port = ntohs(temp.sin_port);
                cout << client_ip << ": " << client_port << " #" << buffer << endl;

                // 添加客户到map中, 如果客户已经存在, 就不做处理
                std::string name(client_ip);
                name += " : ";
                name += to_string(client_port);

                add_user(name, temp);
                // 向队列中push用户发送的消息
                std::string msg = name;
                msg += ">> ";
                msg += buffer;
                _rq.push(msg);
            }
        }

        void server_broadcast()
        {
            // 该任务是从不断的从队列中拿到数据然后发送给其他客户
            while (1)
            {
                std::string send_str;
                _rq.pop(&send_str);

                // 取出所有的sockaddr_in, 存放到vector, 然后遍历vector, 发送数据
                std::vector<sockaddr_in> v;
                {
                    LockGuard lockguard(&_lock);
                    for(auto& user : _user_map)
                    {
                        v.push_back(user.second);
                    }
                }
                for (auto& user : v)
                {
                    sendto(_socket_, send_str.c_str(), send_str.size(), 0, (sockaddr*)&user, sizeof(user));
                }
            }
        }

        void add_user(const std::string &name, const sockaddr_in &sock)
        {
            // 向map中插入用户
            // 为了保证map的线程安全, 需要进行加锁
            LockGuard lockguard(&_lock);
            _user_map[name] = sock;
        }

        ~udp_server()
        {
            pthread_mutex_destroy(&_lock);
            _recv_th->join();
            _broad_th->join();

            delete _recv_th;
            delete _broad_th;
        }

    private:
        int _socket_;
        uint16_t _port_;
        RingQueue<std::string> _rq;
        std::unordered_map<std::string, sockaddr_in> _user_map;
        Thread *_recv_th;
        Thread *_broad_th;
        pthread_mutex_t _lock;
    };

} // namespace Fjl
