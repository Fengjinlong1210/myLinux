#include "RingQueue.hpp"
#include "task.hpp"
#include <ctime>
using namespace std;
string opt = "+-*/%";
void *consumer_thread(void *args)
{
    RingQueue<Task> *rq = static_cast<RingQueue<Task> *>(args);
    cout << "consumer ready" << endl;

    while (1)
    {
        // sleep(1);
        // 消费者从队列中读取数据
        Task t;
        rq->pop(&t);
        t();
        cout << "consumer get : " << t.formatArgs() << t.formatResult() << endl;
    }
}

void *producer_thread(void *args)
{
    RingQueue<Task> *rq = static_cast<RingQueue<Task> *>(args);
    cout << "producer ready" << endl;
    while (1)
    {
        sleep(1);
        int num1 = rand() % 100 + 1;
        int num2 = rand() % 100 + 1;
        int n = rand() % 5;
        char op = opt[n];
        Task t(num1, num2, op);
        rq->push(t);
        cout << "producer push : " << t.formatArgs() << "?" << endl;
    }
}

int main()
{
    srand((unsigned int)time(nullptr));
    // 多对多的生产消费模型
    RingQueue<Task> *rq = new RingQueue<Task>();
    // 将环形队列作为参数传入线程函数
    pthread_t c[3], p[3];
    for (int i = 0; i < 3; ++i)
    {
        pthread_create(c + i, nullptr, consumer_thread, rq);
    }

    for (int i = 0; i < 3; ++i)
    {
        pthread_create(c + i, nullptr, producer_thread, rq);
    }

    for (int i = 0; i < 3; ++i)
    {
        pthread_join(c[i], nullptr);
    }

    for (int i = 0; i < 3; ++i)
    {
        pthread_join(p[i], nullptr);
    }

    return 0;
}

// void *consumer_thread(void *args)
// {
//     RingQueue<int> *rq = static_cast<RingQueue<int> *>(args);
//     cout << "consumer ready" << endl;

//     while (1)
//     {
//         // sleep(1);
//         // 消费者从队列中读取数据
//         int data = 0;
//         rq->pop(&data);
//         cout << "consumer get data : " << data << endl;
//     }
// }

// void *producer_thread(void *args)
// {
//     RingQueue<int> *rq = static_cast<RingQueue<int> *>(args);
//     cout << "producer ready" << endl;
//     while (1)
//     {
//         sleep(1);
//         int data = rand() % 100 + 1;
//         rq->push(data);
//         cout << "producer push data : " << data << endl;
//     }
// }

// int main()
// {
//     srand((unsigned int)time(nullptr));
//     //单对单的生产消费模型
//     RingQueue<int>* rq = new RingQueue<int>();
//     //将环形队列作为参数传入线程函数
//     pthread_t c, p;
//     pthread_create(&c, nullptr, consumer_thread, rq);
//     pthread_create(&c, nullptr, producer_thread, rq);

//     pthread_join(c, nullptr);
//     pthread_join(p, nullptr);
//      delete rq;
//     return 0;
// }
