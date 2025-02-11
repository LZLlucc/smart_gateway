#ifndef APP_BUFFER_H_
#define APP_BUFFER_H_

#include <pthread.h>

/* 子缓冲区 */
typedef struct {
    unsigned char *ptr; /* 内存指针-指向头部 */
    int size;           /* 缓冲区大小 */
    int len;            /* 数据长度 */
} sub_buffer_t;

/* 缓冲区 */
typedef struct {
    sub_buffer_t *sub_buffer[2];    /* 子缓冲区-W 和 R */
    int read_index;                 /* 读索引 */
    int write_index;                /* 写索引 */
    pthread_mutex_t read_lock;      /* 读锁 */
    pthread_mutex_t write_lock;     /* 写锁 */
} buffer_t;

buffer_t *app_buffer_init(int size);
void app_buffer_free(buffer_t *buffer);
int app_buffer_write(buffer_t *buffer, const char *datas, int datas_len);
int app_buffer_read(buffer_t *buffer, char *datas_buffer, int datas_size);

#endif /* APP_BUFFER_H_ */