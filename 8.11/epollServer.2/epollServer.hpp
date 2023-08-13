#pragma once
#include <iostream>
#include <string>
#include <functional>
#include <assert.h>
#include <unordered_map>
#include <unistd.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <sys/types.h>
#include "socket.hpp"
#include "utility.hpp"
#include "epoll.hpp"
#include "protocol.hpp"
class connection;
class epollServer;
class Utility;
using namespace protocol;

// using func_t = std::function<std::string(std::string)>;
using func_t = std::function<Response(const Request &)>;
using callback_t = std::function<void(connection *)>;
// 创建一个类表示一个连接
class connection
{
public:
    connection(int fd, const string client_ip, const uint16_t client_port)
        : _fd(fd), _client_ip(client_ip), _client_port(client_port)
    {
    }

    void Register(callback_t reciver, callback_t sender, callback_t excepter)
    {
        _reciver = reciver;
        _sender = sender;
        _excepter = excepter;
    }

public:
    // IO信息
    int _fd;
    std::string _inbuffer;
    std::string _outbuffer;

    // IO处理函数
    callback_t _reciver;
    callback_t _sender;
    callback_t _excepter;

    // 客户端信息
    std::string _client_ip;
    uint16_t _client_port;

    // 事件集
    uint32_t _events;

    // 回指指针
    epollServer *_r_ptr;
};

class epollServer
{
    static const int gnum = 64;
    const static uint16_t default_port = 8888;

public:
    epollServer(func_t func, uint16_t port = default_port)
        : _func(func), _port(port)
    {
    }
    ~epollServer()
    {
        _epoll.Close();
        _listensock.Close();
    }

    void Init_Server()
    {
        // 初始化套接字
        _listensock.Socket();
        _listensock.Bind(_port);
        _listensock.Listen();
        _epoll.Create();
        logMessage(Debug, "initialize EpollServer success");
        // 要将listensocket设置进数组内
        // listensocket也会被当成读事件就绪
        // 在网络中, 新的连接到来也会被当成读事件就绪

        // 将服务器设置成ET模式
        AddConnection(_listensock.get_socket(), EPOLLIN | EPOLLET);
    }

    void Dispatcher() // 事件派发器, 将已经就绪的事件派发到对应的连接中, 再调用连接的回调函数处理事件
    {
        // int timeout = -1; // 阻塞式
        int timeout = 0; // 非 阻塞式
        while (1)
        {
            LoopOnce(timeout);
        }
    }

    void LoopOnce(int timeout)
    {
        int n = _epoll.Wait(_recv, gnum, timeout);
        for (int i = 0; i < n; ++i)
        {
            int fd = _recv[i].data.fd;
            uint32_t event = _recv[i].events;
            logMessage(Info, "我正在处理%d号文件描述符中的%s事件", fd, (event & EPOLLIN) ? "EPOLLIN" : "OTHER");

            // 将所有的异常全部转化成reciver和sender的异常
            if (event & EPOLLERR || event & EPOLLHUP)
            {
                event |= EPOLLIN | EPOLLOUT;
            }

            if ((event & EPOLLIN) && ConnectionIsExists(fd))
            {
                _connections[fd]->_reciver(_connections[fd]);
            }

            if ((event & EPOLLOUT) && ConnectionIsExists(fd))
            {
                _connections[fd]->_sender(_connections[fd]);
            }
        }
    }

    void AddConnection(const int fd, const uint32_t event, const string &client_ip = "127.0.0.1", const uint16_t client_port = default_port)
    {
        // 0. 设置fd为非阻塞
        if (event & EPOLLET)
        {
            Utility::SetNonBlock(fd);
        }
        // 1. 构建一个connection对象放入map管理
        connection *conn = new connection(fd, client_ip, client_port);
        if (conn->_fd == _listensock.get_socket())
        {
            conn->Register(std::bind(&epollServer::Accepter, this, std::placeholders::_1), nullptr, nullptr);
        }
        else
        {
            conn->Register(
                std::bind(&epollServer::Reciver, this, std::placeholders::_1),
                std::bind(&epollServer::Sender, this, std::placeholders::_1),
                std::bind(&epollServer::Excepter, this, std::placeholders::_1));
        }
        // 将事件写入构建好的connction中
        conn->_events = event;
        conn->_r_ptr = this;
        _connections.emplace(fd, conn);
        // 2. 将新的fd和event写入到内核中
        bool ret = _epoll.AddEvent(fd, EPOLLIN | EPOLLET);
        assert(ret);
        (void)ret;
        logMessage(Debug, "Add Connection success, fd: %d, clientinfo[%s-%d]", fd, client_ip.c_str(), client_port);
    }

    // void Accepter(connection conn)
    void Accepter(connection *conn)
    {
        // 因为设置了非阻塞IO, 所以必须要循环读取, 保证内核所有数据都被读取到
        do
        {
            int err;
            string client_ip;
            uint16_t client_port;
            // 从listensock中获取新连接
            int client_sock = _listensock.Accept(&client_ip, &client_port, &err);
            if (client_sock > 0)
            {
                // 把新建立的连接放到map和内核中
                // 设置fd为ET模式
                AddConnection(client_sock, EPOLLIN | EPOLLET, client_ip, client_port);
                logMessage(Info, "%d-%s已经连上了服务器", client_sock, client_ip.c_str());
            }
            else
            {
                if (err == EWOULDBLOCK || err == EAGAIN)
                {
                    // 底层并没有数据
                    break;
                }
                else if (err == EINTR)
                {
                    // IO被信号中断
                    continue;
                }
                else
                {
                    logMessage(Warning, "Accepter error, errcode: %d, errstring: %s", err, strerror(err));
                    continue; // 继续读取
                }
            }
        } while (conn->_events | EPOLLET);
        logMessage(Info, "Accepter done");
    }

    bool HandleRecive(connection *conn)
    {
        bool ret = true;
        // 轮询方式读取输入
        do
        {
            // logMessage(Info, "EPOLL STATUS: %d", conn->_events);
            char buffer[1024];
            ssize_t n = recv(conn->_fd, buffer, sizeof(buffer) - 1, 0);
            if (n > 0)
            {
                buffer[n] == 0;
                conn->_inbuffer += buffer;
            }
            else if (n == 0)
            {
                // 对端关闭
                conn->_excepter(conn);
                ret = false;
                break;
            }
            else
            {
                if (errno == EWOULDBLOCK || errno == EAGAIN)
                {
                    //logMessage(Info, "数据未就绪, errcode: %d, errstring: %s", errno, strerror(errno));
                    break; // 数据未就绪
                }
                else if (errno == EINTR)
                {
                    continue; // 信号中断
                }
                else
                {
                    conn->_excepter(conn); // 异常
                    ret = false;
                    break;
                }
            }
        } while (conn->_events & EPOLLET);
        return ret;
    }

    void Reciver(connection *conn)
    {
        // 如果在接收过程中出现异常, 直接关闭了连接
        if (!HandleRecive(conn))
            return;
        // 接收后对数据进行处理
        HandleRequest(conn);

        // 当发送缓冲区不为空时, 主动触发一次发送
        if (!conn->_outbuffer.empty())
        {
            conn->_sender(conn);
        }
    }

    void Sender(connection *conn)
    {
        // EPOLLET模式下的写入, 只有当缓冲区在满与不满(也就是不可写与可写)之间切换时
        // 才会触发EPOLLOUT事件
        // 所以要循环写入, 当写入完毕时, 手动关闭写事件
        bool flag = true;
        do
        {
            ssize_t n = send(conn->_fd, conn->_outbuffer.c_str(), conn->_outbuffer.size(), 0);
            if (n > 0)
            {
                // 写入成功, 将发送缓冲区的数据擦除, 防止下次重复发送
                conn->_outbuffer.erase(0, n);
                if (conn->_outbuffer.empty())
                    break;
                // 当缓冲区内数据正好写入完成时, 停止发送
            }
            else
            {
                if (errno == EAGAIN || errno == EWOULDBLOCK)
                {
                    // 证明内核的发送缓冲区已经写满
                    break; // 等待下次读写
                }
                else if (errno == EINTR)
                {
                    // 被信号中断
                    continue;
                }
                else
                {
                    flag = false;
                    conn->_excepter(conn); // 触发异常
                    break;
                }
            }
        } while (conn->_events & EPOLLET);
        if (!flag)
            return;                   // 触发异常了, 原本的conn已经被释放
        if (!conn->_outbuffer.empty()) // 发送完成, 需要关闭写事件
            EnableReadWrite(conn, true, true);
        else // 写入还没有完成, 但是内核的缓冲区被写满, 需要等待取走后继续关心写事件
            EnableReadWrite(conn, true, false);
        logMessage(Debug, "Sender done");
    }

    void Excepter(connection *conn)
    {
        // 出现异常, 关闭连接
        // 1. 先从epoll中移除对连接事件的关心
        int fd = conn->_fd;
        _epoll.DelEvent(fd);

        // 2. 删除哈希表中维护的kv关系
        _connections.erase(fd);

        // 3. 关闭对端描述符
        close(fd);

        // 4. 释放空间
        delete conn;
        logMessage(Info, "fd: %d quit", fd);
        // 因为出现异常后要释放空间, 所以类内的函数要考虑释放后不能对野指针进行访问
    }

    bool ConnectionIsExists(int fd)
    {
        return _connections.count(fd);
    }

    bool EnableReadWrite(connection *conn, bool readable, bool writeable)
    {
        // 控制连接的事件 手动选择可读或可写
        conn->_events = ((readable ? EPOLLIN : 0) | (writeable ? EPOLLOUT : 0) | EPOLLET);
        return _epoll.ModEvent(conn->_fd, conn->_events);
    }

    void HandleRequest(connection *conn)
    {
        bool flag = false;
        while (!flag)
        {
            string requestStr;
            // 1. 截取报文, requestStr是整个报文, 包含报头和有效载荷
            int len = readPackage(conn->_inbuffer, &requestStr); // len表示有效载荷的长度
            logMessage(Debug, "package length: %d", len);
            if (len > 0)
            {
                // 2. 截取有效载荷, 移除报头
                requestStr = removeHeader(requestStr, len);
                // 3. 创建一个请求, 对请求str中的内容进行反序列化, 得到完成的可以被处理的请求
                Request req;
                req.deserialize(requestStr);
                // 4. 对请求进行处理, 得到结果
                Response resp = _func(req);
                // 5. 对结果进行序列化
                string responseStr;
                resp.serialize(&responseStr);
                // 6. 添加报头
                responseStr = addHeader(responseStr);
                logMessage(Debug, "response string: %s", responseStr.c_str());
                // 7. 返回字符串, 将结果添加到连接的发送缓冲区
                conn->_outbuffer += responseStr;
            }
            else
            {
                // 读取不完整或已经读取完毕, 会退出
                flag = true;
            }
        }
        logMessage(Debug, "Handle request done");
    }

private:
    sock _listensock;
    uint16_t _port;
    struct epoll_event _recv[gnum];
    // 设置一个数组用来保存所有的已经就绪的事件结构体
    Epoll _epoll;
    func_t _func;
    std::unordered_map<int, connection *> _connections;
};
