#ifndef APP_POOL_H_
#define APP_POOL_H_

typedef int (*task_func)(void *argv);
/* 任务结构体 */
typedef struct{
    task_func func; /* 任务函数 */
    void *arg;                    /* 任务参数 */
}task_t;

int app_pool_init(int max_task_num);
void app_pool_close();
int app_pool_register_task(task_t *task);

#endif /* APP_POOL_H_ */
