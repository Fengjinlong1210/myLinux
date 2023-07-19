#pragma once
#include <iostream>
#include <pthread.h>
using namespace std;

class Lock
{
public:
    Lock(pthread_mutex_t *mutex)
        : _mutex(mutex)
    {}

    void lock()
    {
        pthread_mutex_lock(_mutex);
    }

    void unlock()
    {
        pthread_mutex_unlock(_mutex);
    }
private:
    pthread_mutex_t *_mutex;
};

class LockGuard
{
public:
    LockGuard(pthread_mutex_t* mutex)
        :_lock(mutex)
    {
        _lock.lock();
    }

    ~LockGuard()
    {
        _lock.unlock();
    }   
private:
    Lock _lock;
};