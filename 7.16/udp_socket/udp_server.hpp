#pragma once
#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <cerrno>
#include "error.hpp"

namespace Fjl
{
    class udp_server
    {
    public:
        udp_server(uint16_t port = 8080)
            :  _port_(port)
        {
        }

        void server_init()
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
            local.sin_family = AF_INET;              // 协议族
            local.sin_addr.s_addr = INADDR_ANY;          // IP地址
                    //inet_addr 将字符串的ip地址转化成32位int, 同时把主机序列转化成网络序列
                    //云服务器或者 一款服务器, 不要指明一个确定的ip
                    //INADDR_ANY允许udpserver启动时, 绑定本主机的任意ip
            local.sin_port = htons(_port_);     // 端口号

            //    绑定
            // 云服务器不需要绑定ip地址, 需要让服务器自己绑定ip地址
            //  本地的虚拟机或物理机是可以绑定的
            if(bind(_socket_, (struct sockaddr*)&local, sizeof(local)) < 0)
            {
                std::cout << "bind socket error : " << strerror(errno) << std::endl;
                exit(BIND_ERROR);
            }
            std::cout << "bind socket success" << std::endl;
        }   

        void server_start()
        {
            char buffer[2048];
            while(1)
            {
                //定义一个结构体充当输出型参数
                struct sockaddr_in temp;
                socklen_t length = sizeof(temp); //该类型本质上是一个unsigned int
                //阻塞式接受数据
                int n = recvfrom(_socket_, buffer, sizeof(buffer) - 1, 0, (sockaddr*)&temp, &length);
                if(n > 0)
                {
                    buffer[n] = 0;
                }
                else
                {
                    continue;
                }

                //读到以后提取客户端信息
                std::string client_ip = inet_ntoa(temp.sin_addr);//该接口将结构体转化成字符串
                uint16_t client_port = ntohs(temp.sin_port);
                cout << client_ip << ": " << client_port << " #" << buffer << endl;

                //再向客户端发送回信息
                int ret = sendto(_socket_, buffer, strlen(buffer), 0, (sockaddr*)&temp, sizeof(temp));
            }
        }

        ~udp_server()
        {
        }

    private:
        int _socket_;
        uint16_t _port_;
        //std::string _ip_;
    };

} // namespace Fjl
