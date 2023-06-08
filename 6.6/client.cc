#include "comm.hpp"

// int main()
// {
//     key_t k = GetKey();
//     //获取shmid
//     int shmid = GetShm(k, getsize);
//     cout << "client shmid:" << shmid << endl;

//     //关联shm
//     char* start = AttachShm(shmid);
//     sleep(10);
//     //通信

//     //取消关联
//     DetachShm(start);

//     return 0;
// }

int main()
{
    Init init(CLIENT);
    sleep(10);
    return 0;
}