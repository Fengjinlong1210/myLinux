// #pragma once
// #include <iostream>
// #include <pthread.h>
// #include <unistd.h>
// #include <queue>

// template <class T>
// class BlockQueue
// {
// public:
//     BlockQueue(const int capacity = 5)
//         : _capacity(capacity)
//     {
//         pthread_mutex_init(&_mutex, nullptr);
//         pthread_cond_init(&_consumer, nullptr);
//         pthread_cond_init(&_producer, nullptr);
//     }

//     void push(const T &val)
//     {
//         // 生产者push的时候, 要保证队列不为空
//         // 先申请锁
//         pthread_mutex_lock(&_mutex);
//         //进入临界区, 当前持有锁
//         //等待环境变量
//         while(isFull())    //使用while循环, 保证在任何时候都是要符合条件才进行生产
//         {
//             //要让线程等待条件允许, 不能持有锁等待
//             //所以条件变量等待要传入锁, 来释放锁
//             pthread_cond_wait(&_producer, &_mutex);
//             //当线程被唤醒后, 继续从wait处向后执行
//             //需要重新申请锁, 申请成功才会彻底返回
//         }
//         //出循环是, 队列不满, 可以进行生产
//         //可以加条件控制队列数量

//         _queue.push(val);
//         //push后, 通知消费者可以进行获取队列内容
//         pthread_cond_signal(&_consumer);
//         pthread_mutex_unlock(&_mutex);
//         //也可在释放锁之后通知, 没有区别, 因为消费者和生产者是互斥的
//     }

//     void pop(T *val)
//     {
//         // 消费者pop获取数据的时候, 要保证队列中有数据
//         pthread_mutex_lock(&_mutex);

//         while(isEmpty())
//         {
//             //等待生产者通知
//             pthread_cond_wait(&_consumer, &_mutex);
//         }
//         //输出型参数
//         *val = _queue.front();
//         _queue.pop();
//         //通知生产者可以继续生产
//         pthread_cond_signal(&_consumer);
//         pthread_mutex_unlock(&_mutex);
//     }

//     bool isFull()
//     {
//         return _queue.size() == _capacity;
//     }

//     bool isEmpty()
//     {
//         return _queue.empty();
//     }

//     ~BlockQueue()
//     {
//         pthread_mutex_destroy(&_mutex);
//         pthread_cond_destroy(&_consumer);
//         pthread_cond_destroy(&_producer);
//     }

// private:
//     // 一个锁、两个条件变量、一个队列
//     std::queue<T> _queue;
//     pthread_mutex_t _mutex;
//     //一把锁是因为队列被当成一份资源来使用
//     pthread_cond_t _consumer;
//     pthread_cond_t _producer;
//     size_t _capacity;
// };

#pragma once

#include <iostream>
#include <queue>
#include <pthread.h>

const int gcap = 5;

// 不要认为，阻塞队列只能放整数字符串之类的
// 也可以放对象
template <class T>
class BlockQueue
{
public:
    BlockQueue(const int cap = gcap):_cap(cap)
    {
        pthread_mutex_init(&_mutex, nullptr);
        pthread_cond_init(&_consumerCond, nullptr);
        pthread_cond_init(&_productorCond, nullptr);
    }
    bool isFull(){ return _q.size() == _cap; }
    bool isEmpty() { return _q.empty(); }
    void push(const T &in)
    {
        pthread_mutex_lock(&_mutex);
        // 细节1：一定要保证，在任何时候，都是符合条件，才进行生产
        while(isFull()) // 1. 我们只能在临界区内部，判断临界资源是否就绪！注定了我们在当前一定是持有锁的！
        {
            // 2. 要让线程进行休眠等待，不能持有锁等待！
            // 3. 注定了，pthread_cond_wait要有锁的释放的能力！
            pthread_cond_wait(&_productorCond, &_mutex); // 我休眠(切换)了，我醒来的时候，在哪里往后执行呢？
            // 4. 当线程醒来的时候，注定了继续从临界区内部继续运行！因为我是在临界区被切走的！
            // 5. 注定了当线程被唤醒的时候，继续在pthread_cond_wait函数出向后运行，又要重新申请锁，申请成功才会彻底返回
        }
        // 没有满的，就要让他进行生产
        _q.push(in);
        // 加策略
        // if(_q.size() >= _cap/2) 
        pthread_cond_signal(&_consumerCond);
        pthread_mutex_unlock(&_mutex);
        // pthread_cond_signal(&_consumerCond);
    }
    void pop(T *out)
    {
        pthread_mutex_lock(&_mutex);
        while(isEmpty()) 
        {
            pthread_cond_wait(&_consumerCond, &_mutex);
        }
        *out = _q.front();
        _q.pop();
        // 加策略
        pthread_cond_signal(&_productorCond);
        pthread_mutex_unlock(&_mutex);
    }
    ~BlockQueue()
    {
        pthread_mutex_destroy(&_mutex);
        pthread_cond_destroy(&_consumerCond);
        pthread_cond_destroy(&_productorCond);
    }
private:
    std::queue<T> _q;
    int _cap;
    // 为什么我们的这份代码，只用一把锁呢，根本原因在于，
    // 我们生产和消费访问的是同一个queue&&queue被当做整体使用！
    pthread_mutex_t _mutex; 
    pthread_cond_t _consumerCond;   // 消费者对应的条件变量，空，wait
    pthread_cond_t _productorCond;  // 生产者对应的条件变量，满，wait
};