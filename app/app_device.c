#include "app_device.h"
#include "log/log.h"
#include "app_common.h"
#include "app_message.h"
#include "app_mqtt.h"
#include "app_buffer.h"
#include "app_pool.h"
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <pthread.h>
#include <string.h>

#define BUFFER_SIZE 1024

static device_t *device = NULL;

device_t *app_device_init(char *filename)
{
    device = (device_t *)malloc(sizeof(device_t));
    if (device == NULL) {
        log_error("malloc device failed");
        return NULL;
    }
    memset(device, 0, sizeof(device_t));

    device->filename = filename;

    device->fd = open(filename, O_RDWR | O_CREAT, 0644);
    if (device->fd < 0) {
        log_error("open %s failed", filename);
        return NULL;
    }

    device->up_buffer = app_buffer_init(BUFFER_SIZE);
    device->down_buffer = app_buffer_init(BUFFER_SIZE);
    device->is_running = 0;
    device->last_write_time = 0;
    device->interval_time = 200;

    return device;
}

/**
 * send_task_func - 发送到远程的任务函数 由线程池中的某个分线池执行
 */
static int send_task_func(void *argv)
{
    /* 从上行缓冲区中读取数据 */
    char datas_buf[128] = {0};
    int len = app_buffer_read(device->up_buffer, datas_buf, sizeof(datas_buf));
    if (-1 == len) {
        log_error("app_buffer_read failed");
        return -1;
    }

    /* 将字符组消息转为json消息 */
    char *json_msg = app_message_chars_to_json(datas_buf, len);

    /* 发送到远程 */
    app_mqtt_publish(json_msg, strlen(json_msg));

    free(json_msg);

    return 0;
}

/**
 * app_device_read - 不断读取下游设备数据
 */
static void *read_thread_func(void *argv)
{
    log_debug("begin to read device from low");
    char read_buf[128] = {0};
    while (device->is_running) {
        memset(read_buf, 0, sizeof(read_buf));
        ssize_t len = read(device->fd, read_buf, sizeof(read_buf));
        // lseek(device->fd, len, SEEK_SET);

        if (-1 == len) {
            perror("read");
            return NULL;
        }

        /* 读取数据后的处理 - 转为字符数组 */
        if (len > 0 && device->post_read) {
            len = device->post_read(read_buf, len);
            if (-1 == len) {
                log_error("post_read failed");
                return NULL;
            }
        }

        /* 将数据写入上行缓冲区 */
        if (len > 0) {
            log_debug("write to up buffer");
            app_buffer_write(device->up_buffer, read_buf, len);

            /* 将数据交给线程池处理 */
            task_t *task = (task_t *)malloc(sizeof(task_t));
            task->func = send_task_func;
            task->arg = NULL;
            app_pool_register_task(task);
        }
    }
    
    return NULL;
}

/* 从下行缓冲区写入数据到设备文件的任务 由线程池中的某个分线程执行 */
static int write_task_func(void *argv)
{
    log_debug("begin to write device from low");
    /* 从下行缓冲区中读取数据 */
    char datas_buf[128] = {0};
    int len = app_buffer_read(device->down_buffer, datas_buf, sizeof(datas_buf));
    if (-1 == len) {
        log_error("app_buffer_read failed");
        return -1;
    }

    /* 将字符组消息转为蓝牙格式的数据 */
    if (len > 0 && device->pre_write) {
        len = device->pre_write(datas_buf, len);
        if (-1 == len) {
            log_error("pre_write failed");
            return -1;
        }
    }
    
    /* 写入设备文件 */
    if (len > 0) {
        /* 如果间隔时间小于200ms，等待 */
        long distance_time = get_current_time() - device->last_write_time;
        if (distance_time < device->interval_time) {
            usleep((device->interval_time - distance_time) * 1000);
        }

        ssize_t w_len = write(device->fd, datas_buf, len);
        log_debug("datas = %s",datas_buf);
        if (w_len < 0) {
            log_error("write failed");
            return -1;
        }
        log_debug("write device succ, the datas : %.*s", w_len, datas_buf);
        device->last_write_time = get_current_time();
        return 0;
    }

    return -1;
}

/* 接收数据处理的回调函数 */
static int recv_msg_cb(char *json, int datas_len)
{
    log_debug("callback");
    /* json -> 字符数组 */
    char datas_buf[128] = {0};
    int len = app_message_json_to_chars(json, datas_buf, sizeof(datas_buf));
    if (-1 == len) {
        log_error("app_message_json_to_chars failed");
        return -1;
    }

    /* 将数据写入下行缓冲区 */
    int res = app_buffer_write(device->down_buffer, datas_buf, len);
    if (-1 == res) {
        log_error("app_buffer_write failed");
        return -1;
    }

    /* 将下行缓冲区数据交给线程池处理 */
    task_t *task = (task_t *)malloc(sizeof(task_t));
    task->func = write_task_func;
    task->arg = NULL;
    app_pool_register_task(task);

    return 0;
}

/**
 * app_device_start - 启动设备
 * 
 * note:
 *      1. 该函数会启动一个线程，用于读取设备数据
 */
int app_device_start()
{
    if (device->is_running) {
        log_info("device is already running");
        return 0;
    }

    device->is_running = 1;

    /* 创建启动读线程 */
    pthread_create(&device->read_pthread , NULL, read_thread_func, NULL);

    /* 注册接收信息处理的回调函数 */
    app_mqtt_register_recv_callback(recv_msg_cb);

    return 0;
}

// void app_device_close()
// {
//     device->is_running = 0;
//     close(device->fd);
//     free(device->up_buffer);
//     free(device->down_buffer);
//     free(device);

// }