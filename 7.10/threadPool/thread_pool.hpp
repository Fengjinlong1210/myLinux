#pragma once
#include <iostream>
#include <pthread.h>
#include <vector>
#include <string>
#include <queue>
#include <unordered_map>
#include <unistd.h>
#include "task.hpp"
#include "LockGuard.hpp"
#include "Thread.hpp"
template <class T>



//threadPool -- version 4
// class threadPool
// {
// private:
//     threadPool(int num = 8)
//         : _thread_cap(num)
//     {
//         //_threads.resize(num);
//         pthread_mutex_init(&_mtx, nullptr);
//         pthread_cond_init(&_cond, nullptr);
//         cout << "get obj" << endl;
//     }

//     threadPool(const threadPool<T>& tp) = delete;
//     threadPool<T>* operator=(const threadPool<T>& tp) = delete;

// public:
//     static threadPool<T>* get_instance()
//     {
//         if(_instance == nullptr)
//         {
//             LockGuard lock(&_static_mtx);
//             if(_instance == nullptr)
//             {
//                 _instance = new threadPool<T>();
//                 _instance->init();
//                 _instance->startRunning();
//             }
//         }
//         return _instance;
//     }

//     ~threadPool()
//     {
//         // 释放所有的线程 -- version -- 2.0
//         for (auto &th : _threads)
//         {
//             th.join();
//         }
//         pthread_mutex_destroy(&_mtx);
//         pthread_cond_destroy(&_cond);
//     }

//     void pushTask(const T &task)
//     {
//         LockGuard lock(&_mtx);
//         // lockQueue();
//         //  上锁, 使线程互斥访问队列
//         _tasks.push(task);
//         // 通知线程已有任务
//         threadWakeup();
//         // unlockQueue();
//     }

//     T popTask() // pop只会在锁内调用, 已经带锁了
//     {
//         // lockQueue();
//         T task = _tasks.front();
//         _tasks.pop();
//         cout << "queue.size = " << _tasks.size() << endl;
//         // unlockQueue();
//         return task;
//     }

//     bool isEmpty()
//     {
//         return _tasks.empty();
//     }

//     static void *thread_func(void *args) // 用static设置成静态成员函数
//     {
//         // 使线程独立, 不用单独回收线程 -- version 1.0
//         // pthread_detach(pthread_self());
//         // 对传入的this指针指向的对象进行处理, 因为static函数不能直接调用类内成员函数
//         threadPool<T> *tp = static_cast<threadPool<T> *>(args);

//         // 线程循环的执行如下操作
//         while (1)
//         {
//             T task;
//             // 有任务就执行, 没有任务就等待
//             {
//                 LockGuard lock(&tp->_mtx);
//                 //tp->lockQueue();
//                 // 为空时等待
//                 while (tp->isEmpty())
//                 {
//                     tp->threadWait();
//                 }
//                 // 获取任务: 把类内队列中的任务加载到函数中
//                 task = tp->popTask(); // 从队列中获取任务, 队列中的任务会被pop
//                 //tp->unlockQueue();
//             }
//             // 处理任务
//             task(); // 调用task类的operator()
//             cout << "Task processing complete : " << task.formatArgs() << task.formatResult() << endl;
//         }
//     }
//     // 因为类内部的函数自带隐含的this指针, 不适合传参, 而static函数不带this指针

//     void init()
//     {
//         int i = 0;
//         for (; i < _thread_cap; ++i)
//         {
//             // 创建自定义的线程对象push进vector
//             _threads.push_back(Thread(i + 1, thread_func, this));
//         }
//     }

//     void startRunning()
//     {
//         for (auto &th : _threads)
//         {
//             th.run(); // 让vector中的每个线程都跑起来
//         }
//     }

//     void check()
//     {
//         for (auto &th : _threads)
//         {
//             if (th.get_status() == RUNNING)
//             {
//                 cout << th.name() << " running ... " << endl;
//             }
//         }
//     }

// private:
//     void lockQueue() // 给队列上锁
//     {
//         pthread_mutex_lock(&_mtx);
//     }

//     void unlockQueue() // 给队列解锁
//     {
//         pthread_mutex_unlock(&_mtx);
//     }

//     void threadWait() // 没有任务时, 阻塞线程
//     {
//         pthread_cond_wait(&_cond, &_mtx);
//     }

//     void threadWakeup() // 唤醒等待任务的线程
//     {
//         pthread_cond_signal(&_cond);
//     }

// private:
//     std::vector<Thread> _threads; // 用于存储多个线程
//     std::queue<T> _tasks;         // 用于存储多个任务
//     int _thread_cap;              // 线程个数
//     pthread_mutex_t _mtx;         // 限制线程访问队列
//     pthread_cond_t _cond;         // 用于线程间同步、按顺序访问任务队列

//     static threadPool<T>* _instance;
//     static pthread_mutex_t _static_mtx;
//     //没有实例化时, 需要一把静态的锁来控制线程实例化时不出现线程安全问题
// };
// template<class T>
// threadPool<T>* threadPool<T>::_instance = nullptr;
// template<class T>
// pthread_mutex_t threadPool<T>::_static_mtx;


// threadPool -- version 3.0
// template <class T>
// class threadPool
// {
// public:
//     threadPool(int num = 8)
//         : _thread_cap(num)
//     {
//         //_threads.resize(num);
//         pthread_mutex_init(&_mtx, nullptr);
//         pthread_cond_init(&_cond, nullptr);
//         cout << "get obj" << endl;
//     }

//     ~threadPool()
//     {
//         // 释放所有的线程 -- version -- 2.0
//         for (auto &th : _threads)
//         {
//             th.join();
//         }
//         pthread_mutex_destroy(&_mtx);
//         pthread_cond_destroy(&_cond);
//     }

//     void pushTask(const T &task)
//     {
//         LockGuard lock(&_mtx);
//         // lockQueue();
//         //  上锁, 使线程互斥访问队列
//         _tasks.push(task);
//         // 通知线程已有任务
//         threadWakeup();
//         // unlockQueue();
//     }

//     T popTask() // pop只会在锁内调用, 已经带锁了
//     {
//         // lockQueue();
//         T task = _tasks.front();
//         _tasks.pop();
//         cout << "queue.size = " << _tasks.size() << endl;
//         // unlockQueue();
//         return task;
//     }

//     bool isEmpty()
//     {
//         return _tasks.empty();
//     }

//     static void *thread_func(void *args) // 用static设置成静态成员函数
//     {
//         // 使线程独立, 不用单独回收线程 -- version 1.0
//         // pthread_detach(pthread_self());
//         // 对传入的this指针指向的对象进行处理, 因为static函数不能直接调用类内成员函数
//         threadPool<T> *tp = static_cast<threadPool<T> *>(args);

//         // 线程循环的执行如下操作
//         while (1)
//         {
//             T task;
//             // 有任务就执行, 没有任务就等待
//             {
//                 LockGuard lock(&tp->_mtx);
//                 //tp->lockQueue();
//                 // 为空时等待
//                 while (tp->isEmpty())
//                 {
//                     tp->threadWait();
//                 }
//                 // 获取任务: 把类内队列中的任务加载到函数中
//                 task = tp->popTask(); // 从队列中获取任务, 队列中的任务会被pop
//                 //tp->unlockQueue();
//             }
//             // 处理任务
//             task(); // 调用task类的operator()
//             cout << "Task processing complete : " << task.formatArgs() << task.formatResult() << endl;
//         }
//     }
//     // 因为类内部的函数自带隐含的this指针, 不适合传参, 而static函数不带this指针

//     void init()
//     {
//         int i = 0;
//         for (; i < _thread_cap; ++i)
//         {
//             // 创建自定义的线程对象push进vector
//             _threads.push_back(Thread(i + 1, thread_func, this));
//         }
//     }

//     void startRunning()
//     {
//         for (auto &th : _threads)
//         {
//             th.run(); // 让vector中的每个线程都跑起来
//         }
//     }

//     void check()
//     {
//         for (auto &th : _threads)
//         {
//             if (th.get_status() == RUNNING)
//             {
//                 cout << th.name() << " running ... " << endl;
//             }
//         }
//     }

// private:
//     void lockQueue() // 给队列上锁
//     {
//         pthread_mutex_lock(&_mtx);
//     }

//     void unlockQueue() // 给队列解锁
//     {
//         pthread_mutex_unlock(&_mtx);
//     }

//     void threadWait() // 没有任务时, 阻塞线程
//     {
//         pthread_cond_wait(&_cond, &_mtx);
//     }

//     void threadWakeup() // 唤醒等待任务的线程
//     {
//         pthread_cond_signal(&_cond);
//     }

// private:
//     std::vector<Thread> _threads; // 用于存储多个线程
//     std::queue<T> _tasks;         // 用于存储多个任务
//     int _thread_cap;              // 线程个数
//     pthread_mutex_t _mtx;         // 限制线程访问队列
//     pthread_cond_t _cond;         // 用于线程间同步、按顺序访问任务队列
// };

// thread_pool version -- 2.0
// template <class T>
// class threadPool
// {
// public:
//     threadPool(int num = 8)
//         : _thread_cap(num)
//     {
//         //_threads.resize(num);
//         pthread_mutex_init(&_mtx, nullptr);
//         pthread_cond_init(&_cond, nullptr);
//         cout << "get obj" << endl;
//     }

//     ~threadPool()
//     {
//         // 释放所有的线程 -- version -- 2.0
//         for (auto &th : _threads)
//         {
//             th.join();
//         }
//         pthread_mutex_destroy(&_mtx);
//         pthread_cond_destroy(&_cond);
//     }

//     void pushTask(const T &task)
//     {
//         lockQueue();
//         // 上锁, 使线程互斥访问队列
//         _tasks.push(task);
//         // 通知线程已有任务
//         threadWakeup();
//         unlockQueue();
//     }

//     T popTask() // pop只会在锁内调用, 已经带锁了
//     {
//         // lockQueue();
//         T task = _tasks.front();
//         _tasks.pop();
//         cout << "queue.size = " << _tasks.size() << endl;
//         // unlockQueue();
//         return task;
//     }

//     bool isEmpty()
//     {
//         return _tasks.empty();
//     }

//     static void *thread_func(void *args) // 用static设置成静态成员函数
//     {
//         // 使线程独立, 不用单独回收线程 -- version 1.0
//         // pthread_detach(pthread_self());
//         // 对传入的this指针指向的对象进行处理, 因为static函数不能直接调用类内成员函数
//         threadPool<T> *tp = static_cast<threadPool<T> *>(args);

//         //线程循环的执行如下操作
//         while (1)
//         {
//             // 有任务就执行, 没有任务就等待

//             tp->lockQueue();
//             // 为空时等待
//             while (isEmpty())
//             {
//                 tp->threadWait();
//             }
//             // 获取任务: 把类内队列中的任务加载到函数中
//             T task = tp->popTask(); // 从队列中获取任务, 队列中的任务会被pop
//             tp->unlockQueue();
//             // 处理任务
//             task(); // 调用task类的operator()
//             cout << "Task processing complete : " << task.formatArgs() << task.formatResult() << endl;
//         }
//     }
//     // 因为类内部的函数自带隐含的this指针, 不适合传参, 而static函数不带this指针

//     void init()
//     {
//         int i = 0;
//         for (; i < _thread_cap; ++i)
//         {
//             // 创建自定义的线程对象push进vector
//             _threads.push_back(Thread(i + 1, thread_func, this));
//         }
//     }

//     void startRunning()
//     {
//         for (auto &th : _threads)
//         {
//             th.run(); // 让vector中的每个线程都跑起来
//         }
//     }

//     void check()
//     {
//         for (auto &th : _threads)
//         {
//             if (th.get_status() == RUNNING)
//             {
//                 cout << th.name() << " running ... " << endl;
//             }
//         }
//     }

// private:
//     void lockQueue() // 给队列上锁
//     {
//         pthread_mutex_lock(&_mtx);
//     }

//     void unlockQueue() // 给队列解锁
//     {
//         pthread_mutex_unlock(&_mtx);
//     }

//     void threadWait() // 没有任务时, 阻塞线程
//     {
//         pthread_cond_wait(&_cond, &_mtx);
//     }

//     void threadWakeup() // 唤醒等待任务的线程
//     {
//         pthread_cond_signal(&_cond);
//     }

// private:
//     std::vector<Thread> _threads; // 用于存储多个线程
//     std::queue<T> _tasks;         // 用于存储多个任务
//     int _thread_cap;              // 线程个数
//     pthread_mutex_t _mtx;         // 限制线程访问队列
//     pthread_cond_t _cond;         // 用于线程间同步、按顺序访问任务队列
// };

// thread_pool version -- 1.0
// template<class T>
// class threadPool
// {
// public:
//     threadPool(int num = 8)
//     :_thread_cap(num)
//     {
//         _threads.resize(num);
//         pthread_mutex_init(&_mtx, nullptr);
//         pthread_cond_init(&_cond, nullptr);
//         cout << "get obj" << endl;
//     }

//     ~threadPool()
//     {
//         pthread_mutex_destroy(&_mtx);
//         pthread_cond_destroy(&_cond);
//     }

//     void pushTask(const T& task)
//     {
//         lockQueue();
//         //上锁, 使线程互斥访问队列
//         _tasks.push(task);
//         //通知线程已有任务
//         threadWakeup();
//         unlockQueue();
//     }

//     T popTask()     //pop只会在锁内调用, 已经带锁了
//     {
//         //lockQueue();
//         T task = _tasks.front();
//         _tasks.pop();
//         //unlockQueue();
//         return task;
//     }

//     bool isEmpty()
//     {
//         return _tasks.empty();
//     }

//     static void* thread_func(void* args)     //用static设置成静态成员函数
//     {
//         //使线程独立, 不用单独回收线程
//         pthread_detach(pthread_self());
//         //对传入的this指针指向的对象进行处理, 因为static函数不能直接调用类内成员函数
//         threadPool<T>* tp = static_cast<threadPool<T>*>(args);

//
//         //获取任务: 把类内队列中的任务加载到函数中
//         while(1)
//         {
//              tp->lockQueue();
//              //为空时等待
//              while(isEmpty())
//                  tp->threadWait();
//              T task = tp->popTask();     //从队列中获取任务, 队列中的任务会被pop
//              tp->unlockQueue();
//              //处理任务
//              task();     //调用task类的operator()
//              cout << "Task processing complete : " << task.formatArgs() << task.formatResult() << endl;
//         }
//
//     }
//     //因为类内部的函数自带隐含的this指针, 不适合传参, 而static函数不带this指针

//     void startRunning()
//     {
//         int i = 0;
//         for(; i < _threads.size(); ++i)
//         {
//             pthread_create(&_threads[i], nullptr, thread_func, this);
//         }
//     }
// private:
//     void lockQueue()        //给队列上锁
//     {
//         pthread_mutex_lock(&_mtx);
//     }

//     void unlockQueue()      //给队列解锁
//     {
//         pthread_mutex_unlock(&_mtx);
//     }

//     void threadWait()       //没有任务时, 阻塞线程
//     {
//         pthread_cond_wait(&_cond, &_mtx);
//     }

//     void threadWakeup()     //唤醒等待任务的线程
//     {
//         pthread_cond_signal(&_cond);
//     }
// private:
//     std::vector<pthread_t> _threads;    //用于存储多个线程
//     std::queue<T> _tasks;               //用于存储多个任务
//     int _thread_cap;                    //线程个数
//     pthread_mutex_t _mtx;               //限制线程访问队列
//     pthread_cond_t _cond;               //用于线程间同步、按顺序访问任务队列
// };