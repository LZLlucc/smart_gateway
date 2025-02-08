#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

void *thread_func(void *arg) 
{
    for (int i = 0; i < 5; i++) {
        printf("Thread is running, i = %d\n", i);
        sleep(1);
    }
    return NULL;
}

int main(int argc, char **argv)
{
    printf("Main is running\n");
    pthread_t thread;
    pthread_create(&thread, NULL, thread_func, NULL);
    sleep(3);
    pthread_cancel(thread);

    int *res = NULL;
    pthread_join(thread, (void **)&res);
    if (res == PTHREAD_CANCELED) 
        printf("Thread was canceled\n");
    else 
        printf("Thread was not canceled\n");

    printf("Main is finished\n");
    return 0;
}