#include <iostream>
#include <unistd.h>
#include <ctime>
#include <pthread.h>
#include "task.hpp"
#include "blockQueue.hpp"
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
using namespace std;

void *consumer_func(void *args)
{
    BlockQueue<Task> *bq = static_cast<BlockQueue<Task> *>(args);
    while (1)
    {
        Task t;
        bq->pop(&t);
        t();//执行operator()获得结果
        cout << "consumer: " << t.formatArgs() << t.formatResult() << endl;
    }
}

void *producer_func(void *args)
{
    BlockQueue<Task> *bq = static_cast<BlockQueue<Task> *>(args);
    string str_op = "+-*/%";
    while (1)
    {
        int num1 = rand() % 20 + 1;
        int num2 = rand() % 10 + 1;
        char op = str_op[rand() % str_op.size()];
        Task t(num1, num2, op);
        bq->push(t);
        cout << "producer: " << t.formatArgs() << "?" << endl;
        sleep(3m);
    }
}

int main()
{
    srand((unsigned int)time(nullptr));
    //多对多的模型
    BlockQueue<Task> *bq = new BlockQueue<Task>(20);
    pthread_t c[3], p[3];
    pthread_create(c + 0, nullptr, consumer_func, bq);
    pthread_create(c + 1, nullptr, consumer_func, bq);
    pthread_create(c + 2, nullptr, consumer_func, bq);
    pthread_create(p + 0, nullptr, producer_func, bq);
    pthread_create(p + 1, nullptr, producer_func, bq);
    pthread_create(p + 2, nullptr, producer_func, bq);

    pthread_join(c[0], nullptr);
    pthread_join(c[1], nullptr);
    pthread_join(c[2], nullptr);
    pthread_join(p[0], nullptr);
    pthread_join(p[1], nullptr);
    pthread_join(p[2], nullptr);

    // // 创造单对单的CP模型
    // BlockQueue<Task> *bq = new BlockQueue<Task>(20);

    // pthread_t consumer, producer;

    // pthread_create(&consumer, nullptr, consumer_func, bq);
    // pthread_create(&producer, nullptr, producer_func, bq);

    // pthread_join(consumer, nullptr);
    // pthread_join(producer, nullptr);
    return 0;
}