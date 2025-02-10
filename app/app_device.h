#ifndef APP_DEVICE_H_
#define APP_DEVICE_H_

#include "app_buffer.h"
#include <pthread.h>

typedef struct {
    char *filename;                         /* 文件名 要传递的数据(下游) */
    int fd;                                 /* 文件描述符 */
    buffer_t *up_buffer;                    /* 上行缓冲区 */
    buffer_t *down_buffer;                  /* 下行缓冲区 */
    pthread_t read_pthread;                 /* 读线程标识 */
    int is_running;                         /* 读线程是否在运行 */
    long last_write_time;                   /* 上次写入时间 */
    int interval_time;                      /* 读取间隔时间 */
    int (*post_read)(char *data, int len);  /* 读取数据后的处理 ms */
    int (*pre_write)(char *data, int len);  /* 写入数据前的处理 */
} device_t;

device_t *app_device_init(char *filename);
int app_device_start();
void app_device_close();


#endif /* APP_DEVICE_H_ */