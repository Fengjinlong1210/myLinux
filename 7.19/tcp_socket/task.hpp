#pragma once
#include <iostream>
#include <string>
#include <functional>
using namespace std;

using cb_t = function<void(int, const string &, const uint16_t &)>;

class Task
{
public:
    Task() // 默认构造函数, 消费者需要使用
    {
    }

    Task(int sock, string client_ip, uint16_t client_port, cb_t callback)
        : _sock(sock), _client_ip(client_ip), _client_port(client_port), _callback(callback)
    {
    }

    int operator()()
    {
        _callback(_sock, _client_ip, _client_port);
    }

private:
    int _sock;
    string _client_ip;
    uint16_t _client_port;
    cb_t _callback;
};