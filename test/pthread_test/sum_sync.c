#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include <stdio.h>

int calc(int num)
{
    sleep(1);
    return num * num;
}

int main(int argc, char **argv) 
{
    struct timeval start, end;
    int num[5] = {1, 2, 3, 4, 5};
    int sum = 0;

    gettimeofday(&start, NULL);
    for (int i = 0; i < 5; i++) {
        sum += calc(num[i]);
    }
    gettimeofday(&end, NULL);

    printf("sum = %d\n", sum);
    printf("time = %ld\n", end.tv_sec - start.tv_sec);
}
