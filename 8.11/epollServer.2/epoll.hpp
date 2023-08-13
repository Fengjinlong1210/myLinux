#pragma once
#include <iostream>
#include <cstring>
#include <sys/epoll.h>
#include <stdlib.h>
#include "log.hpp"
#include "error.hpp"

static const int default_epfd = -1;
static const int gsize = 128;
class Epoll
{
public:
    Epoll()
        : _epfd(default_epfd)
    {
    }

    void Create()
    {
        _epfd = epoll_create(128);
        if (_epfd < 0)
        {
            logMessage(Fatal, "epoll create error, errno: %d, errstring: %s", errno, strerror(errno));
            exit(EPOLL_CREATE_ERROR);
        }
    }

    bool AddEvent(int fd, uint32_t event)
    {
        struct epoll_event ev;
        ev.events = event;
        ev.data.fd = fd;
        int n = epoll_ctl(_epfd, EPOLL_CTL_ADD, fd, &ev);
        if (n < 0)
        {
            logMessage(Fatal, "epoll_ctl error, errno: %d, errstring: %s", errno, strerror(errno));
            return false;
        }
        return true;
    }

    bool ModEvent(int fd, uint32_t event)
    {
        struct epoll_event ev;
        ev.events = event;
        ev.data.fd = fd;
        int n = epoll_ctl(_epfd, EPOLL_CTL_MOD, fd, &ev);
        if (n < 0)
        {
            logMessage(Fatal, "epoll_ctl error, errno: %d, errstring: %s", errno, strerror(errno));
            return false;
        }
        return true;
    }

    bool DelEvent(int fd)
    {
        int n = epoll_ctl(_epfd, EPOLL_CTL_DEL, fd, nullptr);
        return n == 0;
    }

    int Wait(struct epoll_event *recv, int max, int timeout)
    {
        // recv用来接收内核拷贝的已经就绪的事件
        // max表示这个event有多大
        int n = epoll_wait(_epfd, recv, max, timeout);
        return n;
    }

    int Fd()
    {
        return _epfd;
    }

    void Close()
    {
        if (_epfd == default_epfd)
            return;
        close(_epfd);
    }

private:
    int _epfd;
};