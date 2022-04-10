/**
 * Example client program that uses thread pool.
 */

#include <stdio.h>
#include <unistd.h>
#include "threadpool.h"

struct data
{
    int a;
    int b;
};

void add(void *param)
{
    struct data *temp;
    temp = (struct data*)param;

    printf("I add two values %d and %d result = %d\n",temp->a, temp->b, temp->a + temp->b);
}

int main(void)
{
    // create some work to do
    struct data work1;
    work1.a = 5;
    work1.b = 10;
    struct data work2;
    work2.a = 50;
    work2.b = 100;
    struct data work3;
    work3.a = 500;
    work3.b = 1000;
    struct data work4;
    work4.a = 5000;
    work4.b = 10000;

    // initialize the thread pool
    pool_init();

    // submit the work to the queue
    pool_submit(&add,&work1);
    pool_submit(&add,&work2);
    pool_submit(&add,&work3);
    pool_submit(&add,&work4);

    // may be helpful 
    sleep(3);

    pool_shutdown();

    return 0;
}