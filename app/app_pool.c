#include "app_pool.h"
#include "log/log.h"
#include <mqueue.h>
#include <pthread.h>
#include <stdlib.h>

static mqd_t mqd;                   /* 消息队列描述符 */
static char *mq_name = "/.test_mq"; /* 消息队列名称 */
static pthread_t *threads;          /* 线程池 */
static int thread_num;              /* 线程数 */

void *thread_func(void *argv)
{
    task_t task = {0};
    while (1) {
        ssize_t size = mq_receive(mqd, (char*)&task, sizeof(task_t), NULL);
        if (sizeof(task_t) == size) {
            task.func(task.arg);
        } else {
            perror("mq_receive");
        }

        free(task.arg);
    }
}

/* 初始化 */
int app_pool_init(int max_task_num)
{
    /* 创建消息队列 */
    struct mq_attr attr;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(task_t);
    mqd = mq_open(mq_name, O_CREAT | O_RDWR, 0644, &attr);
    if (mqd == (mqd_t)-1) {
        perror("mq_open");
        return -1;
    }

    /* 创建线程池 */
    thread_num = max_task_num;
    threads = (pthread_t *)malloc(sizeof(pthread_t) * thread_num);
    if (threads == NULL) {
        perror("malloc");
        return -1;
    }
    for (int i = 0; i < thread_num; i++) {
        pthread_create(&threads[i], NULL, thread_func, NULL);
    }

    return 0;
}

/* 关闭 */
void app_pool_close()
{
    /* 关闭和删除消息队列 */
    mq_close(mqd);
    mq_unlink(mq_name);

    /* 销毁线程 */
    for (int i = 0; i < thread_num; i++) {
        pthread_cancel(threads[i]);
        pthread_join(threads[i], NULL);
    }

    free(threads);
}

/* 注册任务 */
int app_pool_register_task(task_t *task)
{
    task_t task_temp = *task;
    int res = mq_send(mqd, (char *)&task_temp, sizeof(task_t), 0);
    if (res == -1) {
        perror("mq_send");
        return -1;
    }
    free(task);
    return 0;
}

