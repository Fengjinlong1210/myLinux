#pragma once
#include <iostream>
#include <pthread.h>
using namespace std;

typedef void *(*func_t)(void *);

enum ThreadStatus
{
    NEW,
    RUNNING,
    EXITED
};

class Thread
{
public:
    Thread(int number, func_t func, void *args)
        : _tid(0), _thread_func(func), _args(args), _status(NEW)
    {
        char buffer[64];
        snprintf(buffer, 64, "%d号线程", number);
        _name = buffer;
    }

    pthread_t get_id()
    {
        return _tid;
    }

    string name()
    {
        return _name;
    }

    ThreadStatus get_status()
    {
        return _status;
    }
    
    //静态成员函数没有this指针, 只能访问静态成员变量
    //静态成员函数主要为了访问静态变量，但是，不能访问普通成员变量。 静态成员函数的意义，
    //不在于信息共享，数据沟通，而在于管理静态数据成员，完成对静态数据成员的封装。
    static void* call_thread_func(void* args)
    {
        Thread* _this = (Thread*)args;
        
        (*_this)();//仿函数调用传入的回调
        return nullptr;
    }

    void operator()()
    {
        if(_thread_func)
        {
            //cout << name() << ":" 
            _thread_func(_args);
        }
    }

    void run()
    {
        int ret = pthread_create(&_tid, nullptr, call_thread_func, this);
        //传this指针, 可以通过this指针访问成员
        if(ret != 0)
        {
            cout << "create error" << endl;
        }
        _status = RUNNING;
    }

    void join()
    {
        int ret = pthread_join(_tid, nullptr);
        if(ret != 0)
        {
            cout << "join error" << endl;
        }
        _status = EXITED;
    }

private:
    pthread_t _tid;
    func_t _thread_func;
    void *_args;
    string _name;
    ThreadStatus _status;
};

// class Thread
// {
// public:
//     Thread(int number, func_t func, void *args)
//         : _tid(0), _thread_func(func), _args(args), _status(NEW)
//     {
//         char buffer[64];
//         snprintf(buffer, 64, "%d号线程", number);
//         _name = buffer;
//     }

//     pthread_t get_id()
//     {
//         return _tid;
//     }

//     string name()
//     {
//         return _name;
//     }

//     ThreadStatus status()
//     {
//         return _status;
//     }
    
//     void run()
//     {
//         int ret = pthread_create(&_tid, nullptr, _thread_func, _args);
//         if(ret != 0)
//         {
//             cout << "create error" << endl;
//         }
//         _status = RUNNING;
//     }

//     void join()
//     {
//         int ret = pthread_join(_tid, nullptr);
//         if(ret != 0)
//         {
//             cout << "join error" << endl;
//         }
//         _status = EXITED;
//     }

// private:
//     pthread_t _tid;
//     func_t _thread_func;
//     void *_args;
//     string _name;
//     ThreadStatus _status;
// };