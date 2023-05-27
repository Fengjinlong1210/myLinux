#include <iostream>
#include <vector>
#include <unistd.h>

#define CMD1 0
#define CMD2 1
#define CMD3 2

using namespace std;

// 声明函数指针
typedef void (*Func_t)();

void Task1()
{
    cout << "pid:" << getpid() << "我正在执行任务1!" << endl; 
}
void Task2()
{
    cout << "pid:" << getpid() << "我正在执行任务2!" << endl; 
}
void Task3()
{
    cout << "pid:" << getpid() << "我正在执行任务3!" << endl; 
}

class Task
{
public:
    Task()
    {
        _func_table.push_back(Task1);
        _func_table.push_back(Task2);
        _func_table.push_back(Task3);
    }

    void Execute(int cmd)
    {
        if(cmd >= 0 && cmd < _func_table.size())
        {
            _func_table[cmd]();
            //回调函数，通过函数指针调用函数
        }
    }

    vector<Func_t> _func_table;  //vector中存放函数指针
};