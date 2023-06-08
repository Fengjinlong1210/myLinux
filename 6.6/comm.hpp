#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <cerrno>
#include <unistd.h>
#include <cstring>
#include <vector>
#include <cassert>
using namespace std;

#define PATHNAME "." //可自行设置
#define PROJ_ID 0x6666

#define SERVER 0
#define CLIENT 1

const int getsize = 4096;

//先根据路径和项目id获取一个key值
key_t GetKey()
{
    key_t k = ftok(PATHNAME, PROJ_ID);
    if(k == -1)
    {
        cout << "error :" << errno << ":" << strerror(errno) << endl;
        //直接退出
        exit(1);
    }

    return k;
}

string ToHex(int num)
{
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "0x%x", num);
    return buffer;
}

//申请共享内存接口

static int _Shm(key_t key, size_t size, int shmflg)
{
    int ret = shmget(key, size, shmflg);
    if(ret == -1)
    {
        cout << "error :" << errno << ":" << strerror(errno) << endl;
        exit(2);
    }
    cout << "共享内存获取成功" << endl;
    return ret;
}

//server创建shm
int CreateShm(key_t key, size_t size)
{
    umask(0);
    //return _Shm(key, size, IPC_CREAT | IPC_EXCL | 0666);
    return _Shm(key, size, IPC_CREAT | IPC_EXCL);
}

//client获取shmid
int GetShm(key_t key, size_t size)
{
    return _Shm(key, size, IPC_CREAT);
}

//关联shm与进程
char* AttachShm(int shmid)
{
    char* start = (char*)shmat(shmid, nullptr, 0);//可读可写  
    assert(start); 
    return start;
}

//取消关联
void DetachShm(char* start)
{
    int ret = shmdt(start);
    if(ret == -1)
    {
        cout << "shmdt error :" << errno << ":" << strerror(errno) << endl;
    }
    else
    {
        cout << "取消关联成功" << endl;
    }
}

//删除共享内存

void DelShm(int shmid)
{
    int ret = shmctl(shmid, IPC_RMID, nullptr);
    assert(ret != -1);
    (void)ret;
}

class Init
{
public:
    Init(int id):_id(id)
    {
        int k = GetKey();
        cout << "获取key成功" << endl;
        if(id == SERVER)
        {
           _shmid = CreateShm(k, getsize);
           cout << "创建shm成功" << endl;
        }
        else
        {
            _shmid = GetShm(k, getsize);
            cout << "获取shm成功" << endl;
        }
        _start = AttachShm(_shmid);
        cout << "关联进程成功" << endl;
    }

    char* GetStart()
    {
        return _start;
    }

    ~Init()
    {
        DetachShm(_start);

        if(_id == SERVER)
        {
            DelShm(_shmid);
        }
    }
private:
    int _id;
    char* _start;
    int _shmid;
};
