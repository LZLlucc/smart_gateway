#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

void *thread_calc_run(void *argv)
{
    int num = *(int *)argv;
    int *res = (int *)malloc(sizeof(int));

    if (!res) {
        perror("malloc failed");
        return NULL;
    }

    *res = num * num;
    sleep(1);
    return (void *)res;
}


int main(int argc, char **argv)
{
    struct timeval start, end;
    int num[5] = {1, 2, 3, 4, 5};
    int sum[5] = {0};
    int sum_all = 0;

    gettimeofday(&start, NULL);
    pthread_t thread[5];
    for (int i = 0; i < 5; i++) {
        pthread_create(&thread[i], NULL, thread_calc_run, &num[i]);
    }

    for (int i = 0; i < 5; i++) {
        void *res;
        pthread_join(thread[i], &res);
        sum[i] = *(int *)res;
        free(res);
    }

    gettimeofday(&end, NULL);

    for (int i = 0; i < 5; i++) {
        sum_all += sum[i];
    }

    gettimeofday(&end, NULL);
    printf("sum = %d\n", sum_all);
    printf("time = %ld\n", end.tv_sec - start.tv_sec);
}