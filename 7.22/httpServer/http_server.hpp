#pragma once
#include <iostream>
#include <functional>
#include <string>
#include "error.hpp"
#include "log.hpp"
#include "socket.hpp"

using func_t = std::function<std::string(std::string)>;
const uint16_t defaultPort = 8888;

class httpServer;
struct threadData
{
    threadData(const int sock, const string& ip, const uint16_t port, httpServer* psvr)
    :_sock(sock), _ip(ip), _port(port), _psvr(psvr)
    {}
    int _sock;
    std::string _ip;
    uint16_t _port;
    httpServer *_psvr;
};

class httpServer
{
public:
    httpServer(func_t func, uint16_t port = defaultPort)
        : _port(port), _func(func)
    {
    }

    void init_server()
    {
        _listensock.Socket();
        _listensock.Bind(_port);
        _listensock.Listen();
    }

    void start_server()
    {
        for (;;)
        {
            std::string client_ip;
            uint16_t client_port;
            int client_sock = _listensock.Accept(&client_ip, &client_port);
            if (client_sock < 0)
            {
                continue;
            }
            pthread_t tid;
            threadData *ptd = new threadData(client_sock, client_ip, client_port, this);
            pthread_create(&tid, nullptr, threadRoutine, ptd);
        }
    }

    void handlerHttpRequest(int client_sock)
    {
        // 处理客户端请求
        char buffer[4096];
        std::string request;
        ssize_t n = recv(client_sock, buffer, sizeof(buffer), 0);
        if (n > 0)
        {
            logMessage(Info, "读取成功");
            buffer[n] = 0;
            request = buffer;
            //调用处理函数
            std::string response;
            response = _func(request);
            send(client_sock, response.c_str(), response.size(), 0);
        }
        else
        {
            logMessage(Info, "client quit");
        }
    }

private:
    static void *threadRoutine(void *args)
    {
        pthread_detach(pthread_self());
        threadData *ptd = static_cast<threadData *>(args);
        ptd->_psvr->handlerHttpRequest(ptd->_sock);
        close(ptd->_sock);
        delete ptd;
        return nullptr;
    }

private:
    sock _listensock;
    uint16_t _port;
    func_t _func;
};
