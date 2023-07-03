#include <iostream>
#include <unistd.h>
#include <ctime>
#include <pthread.h>
#include "blockQueue.hpp"
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
using namespace std;

void *consumer_func(void *args)
{
    BlockQueue<int> *bq = static_cast<BlockQueue<int> *>(args);
    while (1)
    {
        int *pdata = new int;
        bq->pop(pdata);
        cout << "consumer get :" << *pdata << endl;
        sleep(1);
    }
}

void *producer_func(void *args)
{
    BlockQueue<int> *bq = static_cast<BlockQueue<int> *>(args);
    while (1)
    {
        int data = rand() % 10;
        bq->push(data);
        cout << "producer push : " << data << endl;
        sleep(1);
    }
}

int main()
{
    srand((unsigned int)time(nullptr));
    // 创造单对单的CP模型
    BlockQueue<int> *bq = new BlockQueue<int>();
    pthread_t consumer, producer;

    pthread_create(&consumer, nullptr, consumer_func, bq);
    pthread_create(&producer, nullptr, producer_func, bq);

    pthread_join(consumer, nullptr);
    pthread_join(producer, nullptr);
    return 0;
}