#include "app_buffer.h"
#include "log/log.h"
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

/* 初始化子缓冲区 */
static sub_buffer_t *sub_buffer_init(int size) 
{
    sub_buffer_t *sub_buffer = (sub_buffer_t *)malloc(sizeof(sub_buffer_t));
    if (sub_buffer == NULL) {
        perror("malloc");
        return NULL;
    }

    sub_buffer->ptr = (unsigned char *)malloc(size);
    if (sub_buffer->ptr == NULL) {
        perror("malloc");
        free(sub_buffer);
        return NULL;
    }

    sub_buffer->size = size;
    sub_buffer->len = 0;

    return sub_buffer;    
}

/* 初始化缓冲区 */
buffer_t *app_buffer_init(int size) 
{
    if (size <= 0) {
        log_error("size is invalid");
        return NULL;
    }

    buffer_t *buffer = (buffer_t *)malloc(sizeof(buffer_t));
    if (buffer == NULL) {
        perror("malloc");
        return NULL;
    }

    buffer->sub_buffer[0] = sub_buffer_init(size);
    buffer->sub_buffer[1] = sub_buffer_init(size);

    buffer->read_index = 0;
    buffer->write_index = 1;

    pthread_mutex_init(&buffer->read_lock, NULL);
    pthread_mutex_init(&buffer->write_lock, NULL);

    return buffer;
}

/* 释放缓冲区 */
void app_buffer_free(buffer_t *buffer)
{
    if (!buffer) {
        return;
    }
    free(buffer->sub_buffer[0]->ptr);
    free(buffer->sub_buffer[1]->ptr);
    free(buffer->sub_buffer[0]);
    free(buffer->sub_buffer[1]);
    pthread_mutex_destroy(&buffer->read_lock);
    pthread_mutex_destroy(&buffer->write_lock);
    free(buffer);
}

/**
 * app_buffer_write - 写缓冲区
 * 
 * note : 缓冲区数据格式 ： 数据长度 +  数据
 * 
 * @buffer[in]: 缓冲区
 * @data[in]: 数据
 * @len[in]: 数据长度
 * 
 * return: 是否写入成功
 *      0: 成功
 *     -1: 失败
 */
int app_buffer_write(buffer_t *buffer, const char *datas, int datas_len) 
{
    if (!buffer || !datas || datas_len <= 0 || datas_len > 255) {
        log_error("param is invalid");
        return -1;
    }

    log_debug("begin to write buffer");

    pthread_mutex_lock(&buffer->write_lock);

    /* 获取写子缓冲区 */
    sub_buffer_t *w_buffer = buffer->sub_buffer[buffer->write_index];

    /* 判断数据是否可以写得入 */
    if (datas_len > w_buffer->size - w_buffer->len - 1) {
        log_error("no enough space to write");
        pthread_mutex_unlock(&buffer->write_lock);
        return -1;
    }

    /* 写数据 */
    w_buffer->ptr[w_buffer->len] = datas_len;
    memcpy(w_buffer->ptr + w_buffer->len + 1, datas, datas_len);

    w_buffer->len += datas_len + 1;

    /* 解锁 */
    pthread_mutex_unlock(&buffer->write_lock);

    log_debug("end to write buffer");

    return 0;
}

/* 切换缓冲区 */
static void switch_buffer(buffer_t *buffer) 
{
    log_debug("switch buffer");
    pthread_mutex_lock(&buffer->write_lock);
    int temp = buffer->read_index;
    buffer->read_index = buffer->write_index;
    buffer->write_index = temp;
    pthread_mutex_unlock(&buffer->write_lock);
}

/**
 * app_buffer_read - 读缓冲区
 *
 * note : 缓冲区数据格式 ： 数据长度 +  数据
 * 
 * @buffer[in]: 缓冲区
 * @data_buffer[in]: 接收数据的缓冲区
 * @data_size[in]: 接收数据的缓冲区大小
 * 
 * return: 读取数据的长度 失败返回-1
 */
int app_buffer_read(buffer_t *buffer, char *datas_buffer, int datas_size) 
{
    if (!buffer || !datas_buffer || datas_size <= 0) {
        log_error("param is invalid");
        return -1;
    }

    log_debug("begin to read buffer");

    pthread_mutex_lock(&buffer->read_lock);

    /* 获取读子缓冲区 */
    sub_buffer_t *r_buffer = buffer->sub_buffer[buffer->read_index];
    if (!r_buffer->len) {
        switch_buffer(buffer);
        r_buffer = buffer->sub_buffer[buffer->read_index];
        if (!r_buffer->len) {
            log_error("no data to read");
            pthread_mutex_unlock(&buffer->read_lock);
            return -1;
        }
    }

    /* 获取数据长度和数据 */
    int datas_len = r_buffer->ptr[0];
    if (datas_len > datas_size) {
        log_error("no enough space to read");
        pthread_mutex_unlock(&buffer->read_lock);
        return -1;
    }
    memcpy(datas_buffer, r_buffer->ptr + 1, datas_len);

    /* 更新缓冲区 */
    r_buffer->len -= datas_len + 1;
    memmove(r_buffer->ptr, r_buffer->ptr + datas_len + 1, r_buffer->len);

    /* 读取完成后 若读子缓冲区为空 切换缓冲区 */
    if (!r_buffer->len) {
        switch_buffer(buffer);
    }

    /* 解锁 */
    pthread_mutex_unlock(&buffer->read_lock);

    return datas_len;
}
