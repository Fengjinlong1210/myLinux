#include <iostream>
#include <unistd.h>
#include <cstring>
#include <cerrno>
#include <stdlib.h>
#include <vector>
#include <ctime>
#include "Task.hpp"
using namespace std;

#define CHILD_NUM 5
#define TASK_NUM 3
//使用父进程创建多个子进程，通过管道向子进程传输任务，让子进程执行

Task t;

class EndPoint
{
public:
    EndPoint(pid_t pid, int wid)
        :_child_pid(pid), _write_id(wid)
    {}

    ~EndPoint()
    {}

    pid_t _child_pid;
    int _write_id;
};

void WaitCmd()
{
    while(1)
    {
        int cmd = 0;
        int readret = read(0, &cmd, sizeof(int));
        if(readret == sizeof(int))
        {
            t.Execute(cmd);
        }
        else
        {
            break;
        }
    }
}

void CreatProcesses(vector<EndPoint>* end_points)
{
    int i = 0;
    for(i = 0; i < CHILD_NUM; ++i)
    {
        // 1.1 创建管道
        int pipefd[2] = {0};
        int piperet = pipe(pipefd);
        if(piperet != 0)
        {//接口调用失败
            cout << "error! errno: " << errno << endl;
            return;
        }
        // 1.2 创建子进程
        pid_t forkret = fork();
        if(forkret == 0)
        {//子进程
            close(pipefd[1]);
            //让子进程的管道文件内容直接输入到0号文件描述符中
            dup2(pipefd[0], 0);
           
            WaitCmd();

            close(pipefd[0]);
            exit(0);
        }

        //父进程
        close(pipefd[0]);
        //向子进程输入数据

        //把子进程的管道文件描述符和pid存入vector
        end_points->push_back(EndPoint(forkret, pipefd[1]));
    }
}

int main()
{
    //srand((unsigned int)time(NULL));
    // 1.先创建出多个子进程，并通过管道与父进程进行通信
    //   父进程写入任务，子进程读取任务
    vector<EndPoint> end_points;
    CreatProcesses(&end_points);
    //通过creatprocesses函数，获得了一个vector
    //这个vector存储的是父进程与子进程进行通信的管道以及子进程的pid

    //向子进程写入数据
    while(1)
    {
        //选择任务
        int tasknum = rand() % TASK_NUM;

        //选择进程
        int procs = rand() % end_points.size();

        //向进程写入任务
        write(end_points[procs]._write_id, &tasknum, sizeof(tasknum));

        sleep(1);
    }
}