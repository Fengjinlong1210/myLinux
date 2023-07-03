#pragma once
#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <queue>

template <class T>
class BlockQueue
{
public:
    BlockQueue(const int capacity = 5)
        : _capacity(capacity)
    {
        pthread_mutex_init(&_mutex, nullptr);
        pthread_cond_init(&_consumer, nullptr);
        pthread_cond_init(&_producer, nullptr);
    }

    void push(const T &val)
    {
        // 生产者push的时候, 要保证队列不为空
        // 先申请锁
        pthread_mutex_lock(&_mutex);
        //进入临界区, 当前持有锁
        //等待环境变量
        while(isFull())    //使用while循环, 保证在任何时候都是要符合条件才进行生产
        {
            //要让线程等待条件允许, 不能持有锁等待
            //所以条件变量等待要传入锁, 来释放锁
            pthread_cond_wait(&_producer, &_mutex);
            //当线程被唤醒后, 继续从wait处向后执行
            //需要重新申请锁, 申请成功才会彻底返回
        }
        //出循环是, 队列不满, 可以进行生产
        //可以加条件控制队列数量

        _queue.push(val);
        //push后, 通知消费者可以进行获取队列内容
        pthread_cond_signal(&_consumer);
        pthread_mutex_unlock(&_mutex);
        //也可在释放锁之后通知, 没有区别, 因为消费者和生产者是互斥的
    }

    void pop(T *val)
    {
        // 消费者pop获取数据的时候, 要保证队列中有数据
        pthread_mutex_lock(&_mutex);

        while(isEmpty())
        {
            //等待生产者通知
            pthread_cond_wait(&_consumer, &_mutex);
        }
        //输出型参数
        *val = _queue.front();
        _queue.pop();
        //通知生产者可以继续生产
        pthread_mutex_unlock(&_mutex);
    }

    bool isFull()
    {
        return _queue.size() == _capacity;
    }

    bool isEmpty()
    {
        return _queue.empty();
    }

    ~BlockQueue()
    {
        pthread_mutex_destroy(&_mutex);
        pthread_cond_destroy(&_consumer);
        pthread_cond_destroy(&_producer);
    }

private:
    // 一个锁、两个条件变量、一个队列
    std::queue<T> _queue;
    pthread_mutex_t _mutex;
    //一把锁是因为队列被当成一份资源来使用
    pthread_cond_t _consumer;
    pthread_cond_t _producer;
    size_t _capacity;
};
