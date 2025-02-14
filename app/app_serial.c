#define _GNU_SOURCE
#include "app_serial.h"
#include "log/log.h"
#include <stdio.h>

int app_serial_init(device_t *dev)
{
    app_serial_set_braudrate(dev, BR_9600);  /* 因为蓝牙模块的初始波特率为9600 */

    app_serial_set_parity(dev, PARITY_NONE);
    app_serial_set_stopbit(dev, SB_1);
    app_serial_set_raw(dev);

    if (tcflush(dev->fd, TCSAFLUSH) != 0) {
        perror("tcflush");
        return -1;
    }

    log_debug("init serial success");
    return 0;
}

/* 设置波特率 */
int app_serial_set_braudrate(device_t *dev, braudrate_t baudrate)
{
    struct termios attr;
    if (tcgetattr(dev->fd, &attr) != 0) {
        perror("tcgetattr");
        return -1;
    }
    cfsetspeed(&attr, baudrate);

    /* 设置串口属性 TCSAFLUSH-当前不生效 刷新时才生效*/
    int ret = tcsetattr(dev->fd, TCSAFLUSH, &attr);
    if (ret != 0) {
        perror("tcsetattr");
        return -1;
    }

    log_debug("set baudrate success");
    return 0;
}

/* 设置校验位 */
int app_serial_set_parity(device_t *dev, parity_t parity)
{
    struct termios attr;
    if (tcgetattr(dev->fd, &attr) != 0) {
        perror("tcgetattr");
        return -1;
    }
    attr.c_cflag &= ~(PARENB | PARODD);
    attr.c_cflag |= parity;

    if (tcsetattr(dev->fd, TCSAFLUSH, &attr) != 0) {
        perror("tcsetattr");
        return -1;
    }
    log_debug("set parity success");
    return 0;
}

/* 设置停止位 */
int app_serial_set_stopbit(device_t *dev, stopbit_t stopbit)
{
    struct termios attr;
    if (tcgetattr(dev->fd, &attr) != 0) {
        perror("tcgetattr");
        return -1;
    }
    attr.c_cflag &= ~CSTOPB;
    attr.c_cflag |= stopbit;

    if (tcsetattr(dev->fd, TCSAFLUSH, &attr) != 0) {
        perror("tcsetattr");
        return -1;
    }
    log_debug("set stopbit success");
    return 0;
}

/* 设置是否为阻塞模式 */
int app_serial_set_block(device_t *dev, int block)
{
    struct termios attr;
    if (tcgetattr(dev->fd, &attr) != 0) {
        perror("tcgetattr");
        return -1;
    }

    if (1 == block) {
        attr.c_cc[VMIN] = 1;    /* 至少读取一个字符 */
        attr.c_cc[VTIME] = 0;   /* 没有数据一直等待 */       
    } else {
        attr.c_cc[VMIN] = 0;    /* 可以没字符 */
        attr.c_cc[VTIME] = 2;   /* 等待0.2s */
    }

    if (tcsetattr(dev->fd, TCSAFLUSH, &attr) != 0) {
        perror("tcsetattr");
        return -1;
    }

    log_debug("set block success");
    return 0;
}

/* 设置为原始模式 */
int app_serial_set_raw(device_t *dev)
{
    struct termios attr;
    if (tcgetattr(dev->fd, &attr) != 0) {
        perror("tcgetattr");
        return -1;
    }

    cfmakeraw(&attr);
    if (tcsetattr(dev->fd, TCSAFLUSH, &attr) != 0) {
        perror("tcsetattr");
        return -1;
    }
    log_debug("set raw success");
    return 0;
}