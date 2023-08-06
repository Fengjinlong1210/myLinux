#pragma once
#include <iostream>
#include <cstring>
#include <cerrno>
#include <string>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "log.hpp"
#include "error.hpp"
using namespace std;
// 封装socket的一系列接口, 既可以由服务器端使用, 也可以由客户端使用
//  1. 创建套接字
//  2. bind
//  3. listen
//  4. accept
//  5. 客户端connect
//  6. 返回socket fd
static const int backlog = 32;

static const int defaultsock = -1;

class sock
{
public:
    sock()
        : _sock(defaultsock)
    {
    }

    void Socket()
    {
        _sock = socket(AF_INET, SOCK_STREAM, 0);
        if (_sock < 0)
        {
            logMessage(Fatal, "create socket error, code: %d, error message: %s", errno, strerror(errno));
            exit(SOCKET_ERROR);
        }
        int opt = 1;
        setsockopt(_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    }

    void Bind(const uint16_t port)
    {
        struct sockaddr_in local;
        memset(&local, 0, sizeof(local));
        local.sin_port = htons(port);
        local.sin_family = AF_INET;
        local.sin_addr.s_addr = INADDR_ANY;

        if (bind(_sock, (sockaddr *)&local, sizeof(local)) < 0)
        {
            logMessage(Fatal, "bind socket error, code: %d, error message: %s", errno, strerror(errno));
            exit(BIND_ERROR);
        }
    }

    void Listen()
    {
        if (listen(_sock, backlog) < 0)
        {
            logMessage(Fatal, "listen socket error, code: %d, error message: %s", errno, strerror(errno));
            exit(LISTEN_ERROR);
        }
    }

    int Accept(string *client_ip, uint16_t *client_port)
    {
        struct sockaddr_in client;
        socklen_t length;
        int n = accept(_sock, (sockaddr *)&client, &length);
        if (n < 0)
        {
            logMessage(Fatal, "accept socket error, code: %d, error message: %s", errno, strerror(errno));
        }
        else
        {
            *client_ip = inet_ntoa(client.sin_addr);
            *client_port = ntohs(client.sin_port);
        }
        return n;
    }

    int Connect(const string &server_ip, const uint16_t &server_port)
    {
        struct sockaddr_in server;
        memset(&server, 0, sizeof(server));
        server.sin_addr.s_addr = inet_addr(server_ip.c_str());
        server.sin_port = htons(server_port);
        server.sin_family = AF_INET;

        return connect(_sock, (sockaddr *)&server, sizeof(server));
    }

    int get_socket()
    {
        return _sock;
    }

    void Close()
    {
        if (_sock != defaultsock)
        {
            close(_sock);
        }
    }

    ~sock()
    {
    }

private:
    int _sock;
};