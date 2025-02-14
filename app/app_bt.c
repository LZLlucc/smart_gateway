#include "app_bt.h"
#include "app_serial.h"
#include "log/log.h"
#include <string.h>
#include <unistd.h>

#define R_BUFFER_SIZE 1024
static char r_buffer[R_BUFFER_SIZE] = {0};
static int r_buffer_len = 0;
static char fix_header[2] = {0xf1, 0xdd};

static int init_bt(device_t *device)
{
    /* 初始化串口 -9600 阻塞 */
    app_serial_init(device);

    app_serial_set_block(device, 0);            /* 非阻塞 */
    tcflush(device->fd, TCIOFLUSH);

    /* 判断蓝牙是否可用 可用则设置属性 */
    if (0 == app_bt_test(device)) {
        app_bt_rename(device, "mesh");
        app_bt_set_baudrate(device, BT_115200);
        // app_bt_set_netid(device, "1369");
        // app_bt_set_maddr(device, "0001");
        app_bt_reset(device);

        /* 等待蓝牙复位 */
        sleep(2);
    }

    /* 改变串口波特率 */
    app_serial_set_braudrate(device, BR_115200);
    tcflush(device->fd, TCIOFLUSH);
    app_bt_set_netid(device, "1369");
    app_bt_set_maddr(device, "0005");

    /* 判断蓝牙是否可用 */
    if (app_bt_test(device)) {
        log_debug("init bt fail");
        return -1;
    }

    log_debug("55555555");
    /* 将串口改为非阻塞 */
    app_serial_set_block(device, 1);
    tcflush(device->fd, TCIOFLUSH);

    log_debug("bt init succ");

    return 0;
}
int app_bt_init(device_t *device)
{
    /* 将蓝牙处理的数据注册到设备中 */
    if (!device) {
        log_debug("bt-device param is invalid");
        return -1;
    }
    device->post_read = app_bt_post_read;
    device->pre_write = app_bt_pre_write;

    /* 初始化蓝牙 */
    int ret = init_bt(device);
    return ret;
}

/* 移除容器的无用数据 */
static void remove_datas(int len)
{
    if (len <= 0 || len > r_buffer_len)
        return;

    memmove(r_buffer, r_buffer + len, r_buffer_len - len);
    r_buffer_len -= len;
}

/**
 * 处理读取的蓝牙数据的函数(蓝牙格式转为字符组格式)
 * datas: 从蓝牙中读取的数据
 * 蓝牙收到的数据的三种情况：
 *   1. “OK\r\n”
 *   2. 蓝牙mesh格式数据
 *   3. 其它无用数据
 * 一次从串口读取的蓝牙数据很可能不是一个完整的数据帧，所以需要将数据帧拼接完整再处理
 * 蓝牙mesh格式数据: f1 dd 07 23 23 ff ff 41 42 43
 *       f1 dd : 固定的头部
 *       07： 之后数据的长度（5-16之间） 
 *       23 23：对端（发送方）的MADDR
 *       ff ff: 我的MADDR或ffff(群发)
 *       41 42 43：发送的数据
 * 要生成的数据：网关中message的二进制数据  conn_type id_len msg_len id msg
 */
int app_bt_post_read(char *datas, int len)
{
    log_debug("bt-recv datas: %.*s", len, datas);

    /* 1.将数据拼接到容器中 */
    memcpy(r_buffer + r_buffer_len, datas, len);
    r_buffer_len += len;
    if (r_buffer_len < 8) 
        return 0;

    /* 2.找到第一个完整的数据 */
    int i = 0;
    for (i = 0; i < r_buffer_len - 7; i++) {
        /* 如果找到 f1dd 则可能是完整的数据 */
        if (0 == memcmp(r_buffer + i, fix_header, 2)) {
            /* 移除没用数据 */
            if (i > 0)
                remove_datas(i);
            
            /* 判断数据是否完整 */
            if (r_buffer_len < 3 + r_buffer[2]) {
                log_debug("datas has not completed, wait");
                return 0;
            }

            /* 3.处理完整的数据- 将数据生成字符数组放在datas中 */
            memset(datas, 0, len);
            datas[0] = 1;                              /* 蓝牙设备 conn_type */
            datas[1] = 2;                              /* id长度 id_len */
            datas[2] = r_buffer[2] - 4;                /* 消息长度 msg_len */
            memcpy(datas + 3, r_buffer + 3, 2);        /* id */
            memcpy(datas + 5, r_buffer + 7, datas[2]); /* msg */

            /* 4.移除已经处理的数据 */
            remove_datas(3 + r_buffer[2]);

            /* 5.返回处理后的数据长度 */
            return 5 + datas[2];
        }
    }

    /* 如果没有找到 f1dd 删除遍历的数据 */
    remove_datas(i);
    return 0;
}

/**
 * 处理将要写入蓝牙数据函数(将字符组数据转化为蓝牙格式)
 *   data: conn_type id_len msg_len id msg
 *   蓝牙发送数据格式例子：
 *       41 54 2b 4d 45 53 48 00 ff ff 41 42 43 0d 0a
 *       41 54 2b 4d 45 53 48 00： AT+MESH（固定头部）
 *       ff ff: 对端的MADDR（如果是FFFF代表群发）
 *       11 22 33: 要发送的数据（不超过12字节）
 *       0d 0a：\r\n（固定结尾）
 */

int app_bt_pre_write(char *datas, int len)
{
    if (len < 6) {
        log_error("datas_len is too short");
        return -1;
    }

    /* 计算数据长度 并创建缓存容器 */
    int bt_len = 12 + datas[2];
    char bt_datas[bt_len];

    /* 构造蓝牙数据包 */
    memcpy(bt_datas, "AT+MESH", 8);
    memcpy(bt_datas + 8, datas + 3, 2);
    memcpy(bt_datas + 10, datas + 5, datas[2]);
    memcpy(bt_datas + 10 + datas[2], "\r\n", 2);

    memset(datas, 0, len);
    memcpy(datas, bt_datas, bt_len);

    log_debug("bt_datas = %s", bt_datas);
    return bt_len;

}

static int wait_ack(device_t *device)
{
    usleep(50 * 1000);

    /* 读取串口数据 */
    char buf[4] = {0};
    read(device->fd, buf, sizeof(buf));
    log_debug("11111111111111: %s", buf);
    if (memcmp(buf, "OK\r\n", 4) == 0)
        return 0;
    else
        return -1;
}

/* 测试蓝牙是否可用 */
int app_bt_test(device_t *device)
{
    write(device->fd, "AT\r\n", 4);

    return wait_ack(device);
}

/* 修改蓝牙名称 */
int app_bt_rename(device_t *device, const char *new_name)
{
    char buf[100] = {0};
    sprintf(buf, "AT+NAME%s\r\n", new_name);
    write(device->fd, buf, strlen(buf));
    return wait_ack(device);
}

/* 设置波特率 */
int app_bt_set_baudrate(device_t *device, bt_braudrate_t baudrate)
{
    char buf[100] = {0};
    sprintf(buf, "AT+BAUD%c\r\n", baudrate);
    write(device->fd, buf, strlen(buf));
    return wait_ack(device);
}

/* 重置蓝牙 */
int app_bt_reset(device_t *device)
{
    write(device->fd, "AT+RESET\r\n", 10);
    return wait_ack(device);
}

/* 设置组网id */
int app_bt_set_netid(device_t *device, const char *netid)
{
    char buf[100] = {0};
    sprintf(buf, "AT+MESHID%s\r\n", netid);
    write(device->fd, buf, strlen(buf));
    return wait_ack(device);
}

/* 设置MAC地址 */
int app_bt_set_maddr(device_t *device, const char *maddr)
{
    char buf[100] = {0};
    sprintf(buf, "AT+MADDR%s\r\n", maddr);
    write(device->fd, buf, strlen(buf));
    return wait_ack(device);
}
