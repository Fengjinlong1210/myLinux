#include <iostream>
#include <unistd.h>
#include <cstring>
#include <cerrno>
#include <stdlib.h>
#include <vector>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctime>
#include "Task.hpp"
using namespace std;

#define EXE_NUM 10
#define CHILD_NUM 5
#define TASK_NUM 3
//使用父进程创建多个子进程，通过管道向子进程传输任务，让子进程执行

Task t;

void ShowTask()
{
    printf("**********************************\n");
    printf("***** 0. Task1   1. Task2    *****\n");
    printf("***** 2. Task3   3. Exit     *****\n");
    printf("**********************************\n");
}

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
    int cnt = 0;
    while(cnt < EXE_NUM)
    {
        int cmd = 0;
        int readret = read(0, &cmd, sizeof(int));
        if(readret == sizeof(int))
        {
            t.Execute(cmd);
        }
        else if(readret == 0)
        {
            //管道的写端关闭
            break;
        }
        else
        {
            break;
        }
        ++cnt;
    }
}

void CreatProcesses(vector<EndPoint>* end_points)
{
    //定义一个数组，存放父进程指向子进程的fd，每次创建新进程时，关闭前面的fd
    //防止子进程继承到父进程给别的子进程写入的fd
    vector<int> fds;
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
            //关闭前面的fd
            for(auto& fd : fds)
            {
                close(fd);
            }
        
            close(pipefd[1]);
            //让子进程的管道文件内容直接输入到0号文件描述符中
            dup2(pipefd[0], 0);
           
            WaitCmd();

            close(pipefd[0]);
            exit(0);
        }

        //父进程:关闭读端
        close(pipefd[0]); 
        //向子进程输入数据
        
        //把子进程的管道文件描述符和pid存入vector
        end_points->push_back(EndPoint(forkret, pipefd[1]));

        fds.push_back(pipefd[1]);
    }
}

void CtrlProcess(vector<EndPoint>& end_points)
{
    int cnt = 0;
    int procs = 0;
    while(cnt < EXE_NUM)
    {
        //选择任务
        //int tasknum = rand() % TASK_NUM;
        int tasknum = 0;
        ShowTask();
        cin >> tasknum;
        if(tasknum < 0 || tasknum > 3)
        {
            cout << "重新输入" << endl;
            continue;
        }
        else if(tasknum == 3)
            break;

        //选择进程

        procs++;
        procs %= CHILD_NUM;
        //向进程写入任务
        write(end_points[procs]._write_id, &tasknum, sizeof(tasknum));

        sleep(1);
        ++cnt;
    }
}

void WaitProcess(vector<EndPoint>& end_points)
{
    //要让子进程退出，父进程需要把写端文件描述符关闭
    //子进程读取时，read会返回0，子进程就会停止等待，进而退出
    // for(auto& ep : end_points)
    // {
    //     int fd = ep._write_id;
    //     close(ep._write_id);
    //     cout << "父进程回收了文件描述符：" << fd << endl;
    // }

    //然后父进程需要回收子进程的僵尸状态
    // for(auto& ep : end_points)
    // {
    //     int pid = ep._child_pid;
    //     waitpid(ep._child_pid, nullptr, 0);//阻塞等待子进程退出
    //     cout << "父进程回收了子进程：" << pid << endl;
    // }

    //将上述两个循环转换成一个循环
    for(int i = 0; i < end_points.size(); ++i)
    {
        int pid = end_points[i]._child_pid;
        int fd = end_points[i]._write_id;

        close(fd);
        cout << "父进程回收了文件描述符：" << fd << endl;
        waitpid(pid, nullptr, 0);
        cout << "父进程回收了子进程：" << pid << endl;
    }
    //这样写会导致第二个子进程无法退出，
    //根本原因是，创建子进程时，第二个以后的子进程继承了之前父进程的fd
    //所以后面的子进程也有fd指向了前面的子进程
    //父进程关闭文件描述符时，子进程的写端仍然有其他的子进程指向
    //所以waitpid函数会一直进行等待

    //解决第二个子进程开始会继承前一个父进程fd的问题
    //实现一个子进程只有父进程才能写入，不能被其他子进程fd指向
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
    CtrlProcess(end_points);
    
    //等待子进程退出
    WaitProcess(end_points);

    return 0;
}
