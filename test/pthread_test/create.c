#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>

pid_t gettid() {
    return syscall(SYS_gettid);
}

void *thread_run(void *argv) {
    int *num = (int *)argv;
    printf("Thread %d is running, the tid is %d\n", *num, gettid());
    free(argv);
    return NULL;
}

int main() {
    pthread_t thread[5];
    printf("main thread %d\n", gettid());
    int i;

    for (i = 0; i < 5; i++) {
        int *num = malloc(sizeof(int));
        *num = i;
        pthread_create(&thread[i], NULL, thread_run, num);
    }

    for (i = 0; i < 5; i++) {
        pthread_join(thread[i], NULL);
    }

    return 0;
}
