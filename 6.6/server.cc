#include "comm.hpp"

int main()
{
    //服务器端创建共享内存
    key_t k = GetKey();
    cout << "key = " << ToHex(k) << endl;
    //sleep(3);

    int shmid = CreateShm(k, getsize);
    cout << "server shmid:" << shmid << endl;
    //sleep(3);

    //关联
    char* start = AttachShm(shmid);
    sleep(10);
    //进行通信

    //取消关联
    DetachShm(start);

    //关闭shm
    DelShm(shmid);

    return 0;
}