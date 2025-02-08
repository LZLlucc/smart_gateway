#define _GNU_SOURCE
#include "app_pool.h"
#include "log/log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


int task_fun(void *argv)
{
    printf("task_func: %s, tid = %d\n", (char *)argv, gettid());
    return 0;
}

int main(int argc, char **argv)
{
    app_pool_init(5);
    for (int i = 0; i < 10; i++) {
        // task_t *task = (task_t *)malloc(sizeof(task_t));
        task_t *task = (task_t *)calloc(1, sizeof(task_t));
        task->func = task_fun;
        char *str = "hello";
        char *a = (char *)malloc(10);
        sprintf(a, "%s_%d", str, i);
        task->arg = a;
        app_pool_register_task(task);
    }

    sleep(1);
    log_debug("main exit");
    app_pool_close();
}
