#include <iostream>
#include <unistd.h>
#include <thread>


using namespace std;

void f1()
{
    int cnt = 5;
    while (cnt--)
    {
        cout << "我是线程1";
        cout << endl;
        sleep(1);
    }
    cout << "1号线程退出" << endl;
}

void f2()
{
    int cnt = 5;
    while (cnt--)
    {
        cout << "我是线程2";
        cout << endl;
        sleep(1);
    }
    cout << "2号线程退出" << endl;
}

void f3()
{
    int cnt = 5;
    while (cnt--)
    {
        cout << "我是线程3";
        cout << endl;
        sleep(1);
    }
    cout << "3号线程退出" << endl;
}

int main()
{
    thread t1(f1);
    thread t2(f2);
    thread t3(f3);

    t1.join();
    t2.join();
    t3.join();

    int cnt = 6;
    while(cnt--)
    {
        cout << "main" << endl;

    }
    return 0;
}

// void run1()
// {
//     while (true)
//     {
//         cout << "thread 1" << endl;
//         sleep(1);
//     }
// }
// void run2()
// {
//     while (true)
//     {
//         cout << "thread 2" << endl;
//         sleep(1);
//     }
// }
// void run3()
// {
//     while (true)
//     {
//         cout << "thread 3" << endl;
//         sleep(1);
//     }
// }

// int main()
// {
//     thread th1(run1);
//     thread th2(run2);
//     thread th3(run3);

//     th1.join();
//     th2.join();
//     th3.join();

//     sleep(5);
//     return 0;
// }