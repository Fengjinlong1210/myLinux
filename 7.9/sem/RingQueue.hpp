#pragma once
#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <vector>
#include <semaphore.h>
using namespace std;

template <class T>
class RingQueue
{
public:
    RingQueue(int num = 8)
        : _Rqueue(num), _capacity(num)
    {
        // 构造函数完成对成员变量的初始化
        //  _Rqueue.resize(num);
        //  _capacity = num;
        sem_init(&_data_sem, 0, 0);
        sem_init(&_space_sem, 0, num);
        pthread_mutex_init(&_c_mtx, nullptr);
        pthread_mutex_init(&_p_mtx, nullptr);
        _c_pos = 0;
        _p_pos = 0;
    }

    ~RingQueue()
    {
        // 析构函数完成销毁
        sem_destroy(&_data_sem);
        sem_destroy(&_space_sem);
        pthread_mutex_destroy(&_c_mtx);
        pthread_mutex_destroy(&_p_mtx);
    }

    void push(const T &data)
    {
        // 生产者push数据, 要先申请空间信号量
        P(_space_sem);
        // 使用生产者锁进行上锁, 让生产者串行push数据
        Lock(_p_mtx);

        _Rqueue[_p_pos++] = data;
        _p_pos %= _capacity;

        Unlock(_p_mtx);
        V(_data_sem);
        // 使用完减少空间信号量, 增加数据信号量
    }

    void pop(T *pdata)
    {
        // 消费者先申请数据信号量
        P(_data_sem);
        Lock(_c_mtx);

        *pdata = _Rqueue[_c_pos++];
        _c_pos %= _capacity;

        Unlock(_c_mtx);
        // 增加空间信号量, 减少数据信号量
        V(_space_sem);
    }

private:
    void P(sem_t& s)
    {
        // 减少/申请信号量
        sem_wait(&s);
    }

    void V(sem_t& s)
    {
        sem_post(&s);
    }

    void Lock(pthread_mutex_t &mtx)
    {
        pthread_mutex_lock(&mtx);
    }

    void Unlock(pthread_mutex_t &mtx)
    {
        pthread_mutex_unlock(&mtx);
    }

private:
    // 用vector模拟环形队列
    vector<T> _Rqueue;
    int _capacity;
    sem_t _data_sem;  // 数据信号量, 只有消费者关心
    sem_t _space_sem; // 空间信号量, 只有生产者关心

    int _c_pos; // 消费者应访问的位置
    int _p_pos; // 生产者应访问的位置

    pthread_mutex_t _c_mtx; // 消费者的锁, 控制消费者进行互斥访问资源
    pthread_mutex_t _p_mtx; // 生产者的锁...
};
