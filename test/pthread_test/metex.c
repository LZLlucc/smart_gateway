#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define PTHREAD_COUNT 10000

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *thread_func(void *argv)
{
    int *p = (int *)argv;

    /* 加锁 */
    pthread_mutex_lock(&mutex);

    (*p)++;

    /* 解锁 */
    pthread_mutex_unlock(&mutex);

    return NULL;
}

int main(int argc, char **argv)
{
    pthread_t thread[PTHREAD_COUNT];
    int sum = 0;

    /* 启动线程对sum进行累加 */
    for (int i = 0; i < PTHREAD_COUNT; i++) {
        pthread_create(&thread[i], NULL, thread_func, &sum);
    }

    /* 等待所有线程结束 */
    for (int i = 0; i < PTHREAD_COUNT; i++) {
        pthread_join(thread[i], NULL);
    }

    /* 打印结果 */
    printf("sum = %d\n", sum);
    
    /* 销毁互斥锁 */
    pthread_mutex_destroy(&mutex);
    
    return 0;
}