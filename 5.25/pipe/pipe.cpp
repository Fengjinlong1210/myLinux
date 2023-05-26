#include <iostream>
#include <unistd.h>
#include <string.h>
#include <cerrno>
using namespace std;

int main()
{
    //1.新建管道文件
    //pipe接口 
    // int pipe(int pipefd[2])   pipe接口的参数是输出型参数
    int pipefd[2] = {0};
    int ret = pipe(pipefd);
    if(ret != 0)
    {
        cout << "error : errorno :" << errno << endl;
    }
    cout << "pipefd[0] : " << pipefd[0] << endl;
    cout << "pipefd[1] : " << pipefd[1] << endl;
 
    //2.创建子进程 : 子进程写入，父进程读取
    int id = fork();
    if(id == 0)//子进程
    {
        //3.删除子进程和父进程对应的fd
        close(pipefd[0]);

        //子进程写入
        const string str = "hello, I am child";
        char buffer[1024];
        int cnt = 1;
        while(1)
        {
            snprintf(buffer, sizeof(buffer), "%s, cnt: %d\n", str.c_str(), cnt++);
            write(pipefd[1], buffer, strlen(buffer));
            sleep(1);
        }

        close(pipefd[1]);
        exit(0);
    }   
    //3.删除子进程和父进程对应的fd
    close(pipefd[1]);

    //4.父子进程开始通信，读取子进程写入的内容
    char buffer[1024];
    while(1)
    {
        int rd = read(pipefd[0], buffer, sizeof(buffer) - 1);
        cout << "I am father, I get :" << buffer << endl;
    }
    return 0;
}