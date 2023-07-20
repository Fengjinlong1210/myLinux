#pragma once
#include <iostream>
#include <cstring>
#include <cerrno>
#include <string>
#include <functional>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include "log.hpp"
#include "socket.hpp"
#include "error.hpp"
#include "protocol.hpp"
// 初始化
// 开始运行
// 执行服务
using namespace protocol;
namespace Fjl
{
    class tcp_server;
    using func_t = std::function<Response(const Request &)>;
    // 将客户端的信息和this指针封住成类, 保证线程能同时获取客户端和类的成员
    struct thread_data
    {
        thread_data(int sock, std::string ip, uint16_t port, tcp_server *p_svr)
            : _sock(sock), _ip(ip), _port(port), _p_svr(p_svr)
        {
        }

        int _sock;
        std::string _ip;
        uint16_t _port;
        tcp_server *_p_svr;
    };

    class tcp_server
    {
    public:
        tcp_server(func_t func, uint16_t port)
            : _port(port)
        {
        }
        void init_server()
        {
            // 初始化封装好的socket
            _listensock.Socket();
            _listensock.Bind(_port);
            _listensock.Listen();
            logMessage(Info, "Init server success, listensocket: %d", _listensock.get_socket());
        }

        void start_server()
        {
            for (;;)
            {
                // 获取客户端的连接请求, 然后创建线程执行任务
                std::string client_ip;
                uint16_t client_port;
                int n = _listensock.Accept(&client_ip, &client_port); // 输出型参数
                if (n < 0)
                    continue;
                // 接受客户端成功
                logMessage(Info, "Accept new client success, client: [%s-%d]", client_ip.c_str(), client_port);
                pthread_t tid;
                thread_data *ptd = new thread_data(n, client_ip, client_port, this);
                pthread_create(&tid, nullptr, threadRoutine, ptd);
            }
        }

        void serviceIO(int sock, const std::string &ip, const uint16_t port)
        {
            // 1.read/recv, 设计一个协议, 保证能够完整的读取到一个string
            // 循环式读取, 一边读取, 一边检测数据完整性
            
            // 2.假设已经读到了完整的字符串, 需要对字符串进行反序列化
            // 也就是解析字符串内容

            // 3.对读取的数据进行处理

            // 4.对处理后的数据进行序列化, 形成字符串,

            // 5.将序列化结果发送到网络, 返回给客户端
        }

    private:
        static void *threadRoutine(void *args)
        {
            pthread_detach(pthread_self());
            thread_data *ptd = static_cast<thread_data *>(args);
            ptd->_p_svr->serviceIO(ptd->_sock, ptd->_ip, ptd->_port);
            delete ptd;
            return nullptr;
        }

    private:
        sock _listensock;
        uint16_t _port;
        func_t func;
    };
}